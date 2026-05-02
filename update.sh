#!/bin/bash
# update11_iterations.sh – MagistralaCAN3: mechanizm iteracji uczenia asocjacyjnego
# Uruchom w katalogu ~/MagistralaCAN3

set -e
echo "🔄 MagistralaCAN3 – iteracje uczenia asocjacyjnego"

if [ ! -f "CMakeLists.txt" ]; then
    echo "❌ Nie znaleziono CMakeLists.txt. Uruchom w ~/MagistralaCAN3"
    exit 1
fi

# ------------------------------------------------------------
# 1. Rozszerzenie AssocLearner o performIterations i filtrowanie
# ------------------------------------------------------------
cp src/ml/assoc_learner.h src/ml/assoc_learner.h.bak_iter_$(date +%Y%m%d_%H%M%S)
cp src/ml/assoc_learner.cpp src/ml/assoc_learner.cpp.bak_iter_$(date +%Y%m%d_%H%M%S)

# Dopisz nowe metody do nagłówka
if ! grep -q "performIterations" src/ml/assoc_learner.h; then
    sed -i '/QSet<uint32_t> candidateIds() const;/a \
    void performIterations(int n, bool eventActive);\
    QVector<Candidate> getFilteredCandidates(int topN = 20, double minScore = 0.1) const;' src/ml/assoc_learner.h
    echo "✔ Nagłówek rozszerzony"
fi

# Dopisz implementację do .cpp
if ! grep -q "AssocLearner::performIterations" src/ml/assoc_learner.cpp; then
    cat >> src/ml/assoc_learner.cpp << 'ITEREOF'

void AssocLearner::performIterations(int n, bool eventActive) {
    for (int i = 0; i < n; ++i) {
        flushWindow();        // używa bieżącego stanu eventActive_
        window_.clear();      // usuń zawartość okna, aby każda iteracja była osobna
    }
    // Po wykonaniu wszystkich iteracji odśwież listę kandydatów
    emit candidatesUpdated();
}

QVector<Candidate> AssocLearner::getFilteredCandidates(int topN, double minScore) const {
    QVector<Candidate> all = getCandidates(topN);
    QVector<Candidate> filtered;
    for (const auto &c : all) {
        if (c.score >= minScore) {
            filtered.append(c);
        }
    }
    return filtered;
}
ITEREOF
    echo "✔ Implementacja dodana"
fi

# ------------------------------------------------------------
# 2. Modyfikacja main.cpp – kontrolki iteracji
# ------------------------------------------------------------
cp src/main.cpp src/main.cpp.bak_iter_$(date +%Y%m%d_%H%M%S)

# Dodaj nowe widgety – QSpinBox i przycisk Iteruj
sed -i '/auto \*eventCheck = new QCheckBox("Zdarzenie");/a \
    auto *iterSpin = new QSpinBox;\
    iterSpin->setRange(1, 100);\
    iterSpin->setValue(1);\
    auto *iterBtn = new QPushButton("Iteruj");' src/main.cpp

sed -i '/controlLayout->addWidget(eventCheck);/a \
    controlLayout->addWidget(new QLabel("Iteracji:"));\
    controlLayout->addWidget(iterSpin);\
    controlLayout->addWidget(iterBtn);' src/main.cpp

# Dodaj #include <QSpinBox> jeśli nie ma
if ! grep -q "#include <QSpinBox>" src/main.cpp; then
    sed -i '/#include <QFileDialog>/a #include <QSpinBox>' src/main.cpp
fi

# Podmieniamy starą lambdę aktualizacji na nową, która filtruje
# Szukamy `QObject::connect(updateCandidatesBtn, ...` i zastępujemy ją
python3 << 'PYFIX'
with open("src/main.cpp", "r") as f:
    content = f.read()

old_connect = """    QObject::connect(updateCandidatesBtn, &QPushButton::clicked, [&]() {
        auto candidates = learner.getCandidates(20);
        candidateListView->setCandidates(candidates);
        QSet<uint32_t> ids;
        for (const auto &c : candidates) ids.insert(c.id);
        model->setHighlightedCandidates(ids);
    });"""

new_connect = """    // Przycisk "Iteruj" – wykonuje iteracje z aktywnym zdarzeniem
    QObject::connect(iterBtn, &QPushButton::clicked, [&]() {
        int n = iterSpin->value();
        bool eventActive = eventCheck->isChecked();

        // Wykonaj iteracje
        learner.setEventActive(eventActive);
        learner.performIterations(n, eventActive);
        learner.setEventActive(false);   // powrót do normalnego trybu

        // Pobierz przefiltrowane wyniki (min score 0.1)
        auto candidates = learner.getFilteredCandidates(20, 0.1);
        candidateListView->setCandidates(candidates);

        // Podświetl tylko istotnych kandydatów
        QSet<uint32_t> ids;
        for (const auto &c : candidates) ids.insert(c.id);
        model->setHighlightedCandidates(ids);

        debugLog(QString("Iteracja: wykonano %1 próbek, znaleziono %2 kandydatów")
                 .arg(n).arg(candidates.size()));
    });"""

content = content.replace(old_connect, new_connect)

# Opcjonalnie – zmień etykietę przycisku "Aktualizuj kandydatów" na "Pokaż wszystkich"
content = content.replace('auto *updateCandidatesBtn = new QPushButton("Aktualizuj kandydatów");',
                          'auto *showAllCandidatesBtn = new QPushButton("Pokaż wszystkich");')
content = content.replace('controlLayout->addWidget(updateCandidatesBtn);',
                          'controlLayout->addWidget(showAllCandidatesBtn);')
# Podepnij ten przycisk do starej funkcji (pobieranie bez filtrowania)
content = content.replace('QObject::connect(updateCandidatesBtn, &QPushButton::clicked, [&]() {',
                          'QObject::connect(showAllCandidatesBtn, &QPushButton::clicked, [&]() {')

with open("src/main.cpp", "w") as f:
    f.write(content)
print("✔ main.cpp zaktualizowany")
PYFIX

# Kompilacja
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo ""
echo "✅ Mechanizm iteracji gotowy."
echo "   Użycie: zaznacz checkbox Zdarzenie, ustaw liczbę iteracji, kliknij Iteruj."
echo "   Lista kandydatów będzie automatycznie filtrowana (próg score = 0.1)."
