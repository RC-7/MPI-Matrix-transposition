#include <vector>
#include <fstream>

typedef struct blockStruct {
    std::vector<std::vector<int>> blockVec;
    int block;
};

void writeBlockVector(std::vector<std::vector<int>> *blocks, int blockSize) {
    std::ofstream outputFile;
    outputFile.open("matrixFile_" + (4*blockSize*blockSize));
    outputFile << (blockSize*2);
    
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < (blockSize*2); ++j) {
            outputFile << " " + (*blocks)[i][j];
        }
    }
}

int main() {
    return 0;
}