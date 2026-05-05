//
// Created by Nguyễn Tuấn Anh on 5/5/26.
//

#include <fstream>

struct PlayerProgress {
    int reachedLevel;
    vector<int> highScores;
    long long lastUpdated;
};

std::string savePath;

void toggleEncryption(char* buffer, size_t size) {
    const char key = 0x5A;
    for (size_t i = 0; i < size; ++i) {
        buffer[i] ^= key;
    }
}

void saveGame(PlayerProgress data) {
    std::ofstream outFile(savePath, std::ios::out | std::ios::binary);
    if (outFile.is_open()) {
        toggleEncryption(reinterpret_cast<char*>(&data.reachedLevel), sizeof(int));
        outFile.write(reinterpret_cast<const char*>(&data.reachedLevel), sizeof(int));

        size_t vectorSize = data.highScores.size();
        outFile.write(reinterpret_cast<const char*>(&vectorSize), sizeof(size_t));

        if (vectorSize > 0) {
            toggleEncryption(reinterpret_cast<char*>(data.highScores.data()), vectorSize * sizeof(int));
            outFile.write(reinterpret_cast<const char*>(data.highScores.data()), vectorSize * sizeof(int));
        }

        toggleEncryption(reinterpret_cast<char*>(&data.lastUpdated), sizeof(long long));
        outFile.write(reinterpret_cast<const char*>(&data.lastUpdated), sizeof(long long));

        outFile.close();
        printf("Saved & Encrypted!");
    }
}

PlayerProgress _loadGame(const std::string& filePath) {
    if (savePath.empty()) {
        savePath = filePath + "player_progress.dat";
    }

    PlayerProgress data;
    std::ifstream inFile(savePath, std::ios::in | std::ios::binary);

    if (inFile.is_open()) {
        inFile.read(reinterpret_cast<char*>(&data.reachedLevel), sizeof(int));
        toggleEncryption(reinterpret_cast<char*>(&data.reachedLevel), sizeof(int));

        size_t vectorSize;
        inFile.read(reinterpret_cast<char*>(&vectorSize), sizeof(size_t));

        data.highScores.resize(vectorSize);
        if (vectorSize > 0) {
            inFile.read(reinterpret_cast<char*>(data.highScores.data()), vectorSize * sizeof(int));
            toggleEncryption(reinterpret_cast<char*>(data.highScores.data()), vectorSize * sizeof(int));
        }

        inFile.read(reinterpret_cast<char*>(&data.lastUpdated), sizeof(long long));
        toggleEncryption(reinterpret_cast<char*>(&data.lastUpdated), sizeof(long long));

        inFile.close();
    } else {
        data.reachedLevel = 1;
        data.highScores.resize(9000);
        for(int i=0; i < 9000; i++) {
            data.highScores[i] = 0;
        }
    }
    return data;
}

void _onUpdatePlayerProgress(bool isWin, int levelId, int score) {
    PlayerProgress data = _loadGame(savePath);

    if (score > data.highScores[levelId]) {
        data.highScores[levelId] = score;
    }

    if (isWin && levelId == data.reachedLevel) {
        data.reachedLevel++;
    }

    data.lastUpdated = time(0);

    saveGame(data);
}