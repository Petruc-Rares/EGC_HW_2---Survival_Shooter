#include "vector"
#include "stack"

class Labirint {
    private:
        int height;
        int width;
        int *matrix;
        void initMatrix();
        bool isValid(int i, int j);
    public:
        Labirint(int height, int width);
        class MatrixCoords {
            public:
                int i;
                int j;
                MatrixCoords(int i, int j);
        };
        MatrixCoords startPos = MatrixCoords(0, 0);
        MatrixCoords endPos = MatrixCoords(0, 0);
        std::vector<Labirint::MatrixCoords> getNeighbors(Labirint::MatrixCoords matrixCoords);   
        void genMaze(MatrixCoords startPos);
        void printLabirint();
        int* getMaze();
};