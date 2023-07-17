#include "string"
#include "vector"
#include "unordered_map"
#include "queue"
#include "cstdlib"
#include "ctime"
#include <iostream>
#include "algorithm"
#include "Labirint.h"
using namespace std;


std::vector<Labirint::MatrixCoords> shuffle(std::vector<Labirint::MatrixCoords> a, int n){
    for (int i = 0; i < n; ++i)
    {
        int rand_idx = rand() % n;
        std::swap(a[i], a[rand_idx]);
    }
    return a;
}


Labirint::Labirint(int height, int width) {
    this->height = height;
    this->width = width;
    this->matrix = new int[height * width];
    initMatrix();

    int randi = 1 + (rand() % (this->height - 2));
    while (randi % 2 == 0) {
        randi = 1 + (rand() % (this->height - 2));
    }
    int randj = 1 + (rand() % (this->width - 2));
    while (randj % 2 == 0) {
        randj = 1 + (rand() % (this->width - 2));
    }

    this->startPos = MatrixCoords(randi, randj);
    cout << "startPos is: " << this->startPos.i << ", " << this->startPos.j << endl;
    
    int i_end = 1 + (rand() % (this->height - 2));
    while (i_end % 2 == 0) {
        i_end = 1 + (rand() % (this->height - 2));
    }
    int j_end;

    float randj_aux = ((double) rand() / (RAND_MAX));

    j_end = (randj_aux > 0.5)? width - 1: 0;
    this->endPos = MatrixCoords(i_end, j_end);
    cout << "endPos is: " << this->endPos.i << ", " << this->endPos.j << endl;

    matrix[this->endPos.i * this->width + this->endPos.j] = 0;

    genMaze(this->startPos);
    // TODO PLACE ENEMIES
    vector<MatrixCoords> neighs =  getNeighbors(startPos);
    for (int i = 0; i < neighs.size(); i++) {
        matrix[neighs[i].i * this->width + neighs[i].j] = 2;
    }
}

void Labirint::genMaze(MatrixCoords startPos) {
    stack<MatrixCoords> stackCoords;

    stackCoords.push(startPos);
    while (!stackCoords.empty()) {
        MatrixCoords crtCell = stackCoords.top();
        
        // make it free way
        matrix[crtCell.i * this->width + crtCell.j] = 0;

        //printLabirint();
        //cout << std::endl;
        stackCoords.pop();
        // if it just a road you can continue;
        if (crtCell.j % 2 == 0) {
            continue;
        }

        vector<MatrixCoords> neighs = getNeighbors(crtCell);
        for (int i = 0; i < neighs.size(); i++) {
            MatrixCoords crtNeighbor = neighs[i];

            // update just if unvisited
            if (matrix[crtNeighbor.i * this->width + crtNeighbor.j] == -1) {
                stackCoords.push(crtCell);
                stackCoords.push(crtNeighbor);

                if (crtNeighbor.i == crtCell.i + 2) {
                    // NORTH
                    stackCoords.push(MatrixCoords(crtCell.i + 1, crtCell.j));
                } else if (crtNeighbor.i == crtCell.i - 2) {
                    // SOUTH
                    stackCoords.push(MatrixCoords(crtCell.i - 1, crtCell.j));
                } else if (crtNeighbor.j == crtCell.j - 2) {
                    // WEST
                    stackCoords.push(MatrixCoords(crtCell.i, crtCell.j - 1));
                } else if (crtNeighbor.j  == crtCell.j + 2) {
                    // EAST
                    stackCoords.push(MatrixCoords(crtCell.i, crtCell.j + 1));
                }
                break;

            }
        }
    }
}

void Labirint::initMatrix() {
    for(int i = 0; i < height; i++) {
        if (i % 2 == 0) {
            for (int j = 0; j < width; j++) {
                this->matrix[j + i * width] = 1;
            }
        } else {
            for (int j = 0; j < width; j++) {
                this->matrix[j + i * width] = (j % 2 == 0)? 1 : -1;
            }
        }
    }    
}


bool Labirint::isValid(int i, int j) {
    return !((i <= 0) || (j <= 0) || (i >= height) || (j >= width));
}

std::vector<Labirint::MatrixCoords> Labirint::getNeighbors(Labirint::MatrixCoords matrixCoords) {
    std::vector<Labirint::MatrixCoords> neighs = {};

    int i = matrixCoords.i;
    int j = matrixCoords.j;

    if (isValid(i - 2, j)) {
        neighs.push_back(MatrixCoords(i - 2, j));
    };

    if (isValid(i + 2, j)) {
        neighs.push_back(MatrixCoords(i + 2, j));
    }

    if (isValid(i, j + 2)) {
        neighs.push_back(MatrixCoords(i, j + 2));
    }

    if (isValid(i, j - 2)) {
        neighs.push_back(MatrixCoords(i, j - 2));
    }

    return shuffle(neighs, neighs.size());
}

void Labirint::printLabirint() {
    for(int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%d ", this->matrix[j + i * width]);
        }
        printf("\n");
    }
}

Labirint::MatrixCoords::MatrixCoords(int i, int j) {
    this->i = i;
    this->j = j;
}

int* Labirint::getMaze() {
    return this->matrix;
}