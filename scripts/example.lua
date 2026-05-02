-- Przykład: loguje każdą ramkę i odpowiada na ID 0x123
function onFrame(id, data, timestamp)
    log(string.format("Ramka: 0x%X, DLC=%d, czas=%.3f", id, #data, timestamp))
    if id == 0x123 then
        sendFrame(0x456, "\xAA\xBB\xCC")
        log("Wysłano odpowiedź na 0x123")
    end
end
