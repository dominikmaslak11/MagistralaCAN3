-- Przykład reguł filtrowania i kolorowania
function onFrame(id, data, timestamp)
    -- Ukryj wszystkie ramki o ID 0x100
    if id == 0x100 then
        hideFrame(id)
        return
    end

    -- Ramki o ID 0x200 pokoloruj na niebiesko
    if id == 0x200 then
        setColor(id, 100, 150, 255)
    end

    -- Ramki, gdzie bajt 0 > 200, pokoloruj na czerwono
    if #data > 0 and data:byte(1) > 200 then
        setColor(id, 255, 100, 100)
    end
end
