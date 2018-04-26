//
//  TigerBlood.h
//  Tiger Blood
//
//  Created by Travis Cox on 3/26/18.
//  Copyright © 2018 Travis Cox. All rights reserved.
//

#ifndef TigerBlood_h
#define TigerBlood_h

#include <vector>
#include <ctime>

using namespace std;

enum Color_t {RED, BLUE};

struct Point_t{
    int row, col;
};

struct Token_t{
    Color_t color;
    Point_t location;
};

struct Move_t{
    Token_t token;
    Point_t destination;
};

struct AdjacencyMatrix{
    Point_t matrix[89][8];
    bool populated = false;
    bool tigerCageMovesInitialized = false;
};

AdjacencyMatrix matrix;
int tigerCageMoves;
const int tigerCageMax = 8;

void populateMatrix(AdjacencyMatrix);
bool inCage(Token_t);
void getAdjacentPoints(vector<Point_t> *, Point_t);
int searchAdjacencyMatrix(Point_t, int, int);
Point_t findNearestHuman(vector<Token_t>, Point_t);
int shortestPath(Point_t, Point_t);
void removeExtraneousPointsTiger(vector<Point_t> *, Point_t, Point_t);
bool checkForTrapping(Point_t, vector<Token_t>);
void getAllAvailableMoves(vector<Token_t>, vector<Move_t> *);
bool checkIfJumpPossible(Point_t, vector<Point_t>::iterator);
bool legalPoint(Point_t);

Move_t  Move_TigerBlood(vector<Token_t> pieces, Color_t turn)
{
    Move_t tempMove;
    vector<Point_t> points;
    if(!matrix.populated)
    {
        populateMatrix(matrix);
    }
    
    if(turn == RED)
    {
        if (!matrix.tigerCageMovesInitialized)
        {
            tigerCageMoves = 0;
            matrix.tigerCageMovesInitialized = true;
        }
        Token_t redToken;
        tempMove.token = redToken;
        vector<Token_t>::iterator tokenIterator = pieces.begin();
        for(int i = 0; i < pieces.size(); i++)
        {
            if(tokenIterator->color == RED)
            {
                redToken = pieces[i];
            }
            tokenIterator.operator++();
        }
        
        getAdjacentPoints(&points, redToken.location);
        
        if(inCage(redToken))
        {
            vector<Point_t>::iterator pointIterator = points.begin();
            if(tigerCageMoves >= tigerCageMax)
            {
                for(int i = 0; i < points.size(); i++)
                {
                    if(pointIterator->row < redToken.location.row)
                    {
                        points.erase(pointIterator);
                        i--;
                    }
                    else
                    {
                        pointIterator++;
                    }
                }
            }
            tigerCageMoves++;
            
            if(points.size() > 1)
            {
                srand(time(NULL));
                int choice = rand() % points.size();
                tempMove.destination = points[choice];
            }
            else
            {
                tempMove.destination = points[0];
            }
        }
        else
        {
            if(checkForTrapping(redToken.location, pieces))
            {
                srand(time(NULL));
                int choice = rand() % points.size();
                tempMove.destination = points[choice];
            }
            else
            {
                Point_t nearestHuman = findNearestHuman(pieces, redToken.location);
                int colDiff, rowDiff;
                colDiff = redToken.location.col - nearestHuman.col;
                rowDiff = redToken.location.row - nearestHuman.row;
                removeExtraneousPointsTiger(&points, redToken.location, nearestHuman);
                bool found = false;
                vector<Point_t>::iterator pointIterator = points.begin();
                for(int i = 0; i < points.size(); i++)
                {
                    if(pointIterator->row != redToken.location.row && pointIterator->col != redToken.location.col)
                    {
                        tempMove.destination = *pointIterator;
                        found = true;
                    }
                    pointIterator++;
                }
                if (!found)
                {
                    if(abs(colDiff) > abs(rowDiff))
                    {
                        pointIterator = points.begin();
                        for(int i = 0; i < points.size(); i++)
                        {
                            if(pointIterator->col == redToken.location.col)
                            {
                                tempMove.destination = *pointIterator;
                            }
                            pointIterator++;
                        }
                    }
                    else
                    {
                        pointIterator = points.begin();
                        for(int i = 0; i < points.size(); i++)
                        {
                            if(pointIterator->row == redToken.location.row)
                            {
                                tempMove.destination = *pointIterator;
                            }
                            pointIterator++;
                        }
                    }
                }
            }
        }
            
    }
    else
    {
        vector<Token_t> blueTokens;
        Token_t redToken;
        vector<Move_t> blueMoves;
        vector<Token_t>::iterator piecesIterator = pieces.begin();
        for(int i = 0; i < pieces.size(); i++)
        {
            if(piecesIterator->color == BLUE)
            {
                blueTokens.push_back(*piecesIterator);
            }
            else
            {
                redToken = *piecesIterator;
            }
            piecesIterator++;
        }
        
        getAllAvailableMoves(blueTokens, &blueMoves);
    }
    
    return tempMove;
}

void getAllAvailableMoves(vector<Token_t> blueTokens, vector<Move_t> *blueMoves)
{
    vector<Token_t>::iterator blueTokenIterator1 = blueTokens.begin(), blueTokenIterator2;
    vector<Point_t> blueAdjacentPoints;
    vector<Point_t>::iterator adjacentIterator;
    for(int i = 0; i < blueTokens.size(); i++)
    {
        getAdjacentPoints(&blueAdjacentPoints, blueTokenIterator1->location);
        blueTokenIterator2 = blueTokens.begin();
        for(int j = 0; j < blueTokens.size(); j++)
        {
            adjacentIterator = blueAdjacentPoints.begin();
            for(int k = 0; k < blueAdjacentPoints.size(); k++)
            {
                if(blueTokenIterator2->location.row == adjacentIterator->row && blueTokenIterator2->location.col == adjacentIterator->col)
                {
                    if(checkIfJumpPossible(blueTokenIterator1->location, adjacentIterator))
                    {
                        blueAdjacentPoints.erase(adjacentIterator);
                        k--;
                    }
                }
                else
                {
                    adjacentIterator++;
                }
            }
            blueTokenIterator2++;
        }
        blueTokenIterator1++;
    }
}

bool checkIfJumpPossible(Point_t me, vector<Point_t>::iterator other)
{
    Point_t tempPoint;
    if(me.row == other->row)
    {
        tempPoint.row = me.row;
        if(me.col > other->col)
        {
            tempPoint.col = other->col-1;
        }
        else
        {
            tempPoint.col = other->col+1;
        }
    }
    else if(me.col == other->col)
    {
        tempPoint.row = me.col;
        if(me.row > other->row)
        {
            tempPoint.row = other->row-1;
        }
        else
        {
            tempPoint.row = other->row+1;
        }
    }
    else if(me.col > other->col)
    {
        tempPoint.col = other->col-1;
        if(me.row > other->row)
        {
            tempPoint.row = other->row-1;
        }
        else
        {
            tempPoint.row = other->row+1;
        }
    }
    else
    {
        tempPoint.row = other->row+1;
        if(me.col > other->col)
        {
            tempPoint.col = other->col-1;
        }
        else
        {
            tempPoint.col = other->col+1;
        }
    }
    
    if(legalPoint(tempPoint))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool legalPoint(Point_t tempPoint)
{
    if(tempPoint.row < 0 || tempPoint.row > 12 || tempPoint.col < 0 || tempPoint.col > 8)
    {
        return false;
    }
    if (tempPoint.row == 0 && tempPoint.col != 4)
    {
        return false;
    }
    if(tempPoint.row == 1 || tempPoint.row == 3)
    {
        if(tempPoint.col != 3 || tempPoint.col != 5)
        {
            return false;
        }
    }
    if(tempPoint.row == 2)
    {
        if(tempPoint.col != 2 && tempPoint.col != 4 && tempPoint.col != 6)
        {
            return false;
        }
    }
    
    return true;
}

bool inCage(Token_t tiger)
{
    Point_t tigerPosition = tiger.location;
    
    if(tigerPosition.row < 4)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool checkForTrapping(Point_t me, vector<Token_t> pieces)
{
    int count = 0, path;
    vector<Token_t>::iterator piecesIterator = pieces.begin();
    for(int i = 0; i < pieces.size(); i++)
    {
        if(piecesIterator->color == BLUE)
        {
            path = shortestPath(me, piecesIterator->location);
            if(path <= 4)
            {
                count++;
            }
        }
        piecesIterator++;
    }
    
    if(count >= 4)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void getAdjacentPoints(vector<Point_t> *points, Point_t me)
{
    int adjacencyMatrixRow = searchAdjacencyMatrix(me, 0, 88), count = 1;
    while (matrix.matrix[adjacencyMatrixRow][count].row != -1)
    {
        points->push_back(matrix.matrix[adjacencyMatrixRow][count]);
        count++;
    }
}

int searchAdjacencyMatrix(Point_t me, int min, int max)
{
    int halfway = (min + max) / 2;
    if(matrix.matrix[halfway][0].col == me.col && matrix.matrix[halfway][0].row == me.row)
    {
        return halfway;
    }
    else if (matrix.matrix[halfway][0].row == me.row)
    {
        if(matrix.matrix[halfway][0].col < me.col)
        {
            return (halfway + (me.col - matrix.matrix[halfway][0].col));
        }
        else
        {
            return (halfway - (matrix.matrix[halfway][0].col - me.col));
        }
    }
    else
    {
        if(matrix.matrix[halfway][0].row < me.row)
        {
            return searchAdjacencyMatrix(me, min, halfway);
        }
        else
        {
            return searchAdjacencyMatrix(me, halfway, max);
        }
    }
}

Point_t findNearestHuman(vector<Token_t> pieces, Point_t me)
{
    vector<Token_t>::iterator piecesIterator = pieces.begin();
    int moves, min = -1;
    Point_t nearest;
    for (int i = 0; i < pieces.size(); i++)
    {
        if (piecesIterator->color == BLUE)
        {
            moves = shortestPath(me, piecesIterator->location);
            if(moves < min || min == -1)
            {
                min = moves;
                nearest = piecesIterator->location;
            }
        }
    }
    
    return nearest;
}

int shortestPath(Point_t me, Point_t destination)
{
    if(me.row == destination.row && me.col == destination.col)
    {
        return 1;
    }
    
    vector<Point_t> adjacentPoints;
    getAdjacentPoints(&adjacentPoints, me);
    int minMoves = -1, moves;
    vector<Point_t>::iterator adjacentIterator = adjacentPoints.begin();
    removeExtraneousPointsTiger(&adjacentPoints, me, destination);
    adjacentIterator = adjacentPoints.begin();
    for(int i = 0; i < adjacentPoints.size(); i++)
    {
        moves = shortestPath(*adjacentIterator, destination);
        if(moves < minMoves || minMoves == -1)
        {
            minMoves = moves;
        }
        adjacentIterator++;
    }
    
    return 1+minMoves;
}

void removeExtraneousPointsTiger(vector<Point_t> *adjacentPoints, Point_t me, Point_t destination)
{
    vector<Point_t>::iterator adjacentIterator = adjacentPoints->begin();
    for (int i = 0; i < adjacentPoints->size(); i++)
    {
        if(me.col < destination.col)
        {
            if(adjacentIterator->col < me.col)
            {
                adjacentPoints->erase(adjacentIterator);
                i--;
            }
            else
            {
                adjacentIterator++;
            }
        }
        else if (me.col > destination.col)
        {
            if(adjacentIterator->col > me.col)
            {
                adjacentPoints->erase(adjacentIterator);
                i--;
            }
            else
            {
                adjacentIterator++;
            }
        }
        else
        {
            if (adjacentIterator->col != me.col)
            {
                adjacentPoints->erase(adjacentIterator);
                i--;
            }
            else
            {
                adjacentIterator++;
            }
        }
    }
    adjacentIterator = adjacentPoints->begin();
    for (int i = 0; i < adjacentPoints->size(); i++)
    {
        if(me.row < destination.row)
        {
            if(adjacentIterator->row < me.row)
            {
                adjacentPoints->erase(adjacentIterator);
                i--;
            }
            else
            {
                adjacentIterator++;
            }
        }
        else if (me.row > destination.row)
        {
            if(adjacentIterator->row > me.row)
            {
                adjacentPoints->erase(adjacentIterator);
                i--;
            }
            else
            {
                adjacentIterator++;
            }
        }
        else
        {
            if (adjacentIterator->row != me.row)
            {
                adjacentPoints->erase(adjacentIterator);
                i--;
            }
            else
            {
                adjacentIterator++;
            }
        }
    }
}
    
void populateMatrix(AdjacencyMatrix m)
{
    Point_t temp;
    temp.row = -1;
    temp.col = -1;
    for(int i = 0; i < 89; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            m.matrix[i][j] = temp;
        }
    }
    
    temp.row = 0;
    temp.col = 4;
    m.matrix[0][0] = temp;
    m.matrix[1][1] = temp;
    m.matrix[2][1] = temp;
    temp.row = 1;
    temp.col = 3;
    m.matrix[1][0] = temp;
    m.matrix[0][1] = temp;
    m.matrix[3][1] = temp;
    m.matrix[4][1] = temp;
    temp.col = 5;
    m.matrix[2][0] = temp;
    m.matrix[0][2] = temp;
    m.matrix[4][2] = temp;
    m.matrix[5][1] = temp;
    temp.row = 2;
    temp.col = 2;
    m.matrix[3][0] = temp;
    m.matrix[1][2] = temp;
    m.matrix[6][1] = temp;
    temp.col = 4;
    m.matrix[4][0] = temp;
    m.matrix[1][3] = temp;
    m.matrix[2][2] = temp;
    m.matrix[6][2] = temp;
    m.matrix[7][1] = temp;
    temp.col = 6;
    m.matrix[5][0] = temp;
    m.matrix[2][3] = temp;
    m.matrix[7][2] = temp;
    temp.row = 3;
    temp.col = 3;
    m.matrix[6][0] = temp;
    m.matrix[3][2] = temp;
    m.matrix[4][3] = temp;
    m.matrix[12][1] = temp;
    temp.col = 5;
    m.matrix[7][0] = temp;
    m.matrix[4][4] = temp;
    m.matrix[5][2] = temp;
    m.matrix[12][2] = temp;
    temp.row = 4;
    temp.col = 0;
    m.matrix[8][0] = temp;
    m.matrix[9][1] = temp;
    m.matrix[17][1] = temp;
    temp.col = 1;
    m.matrix[9][0] = temp;
    m.matrix[8][1] = temp;
    m.matrix[10][1] = temp;
    m.matrix[18][1] = temp;
    temp.col = 2;
    m.matrix[10][0] = temp;
    m.matrix[9][2] = temp;
    m.matrix[11][1] = temp;
    m.matrix[19][1] = temp;
    temp.col = 3;
    m.matrix[11][0] = temp;
    m.matrix[10][2] = temp;
    m.matrix[12][3] = temp;
    m.matrix[20][1] = temp;
    temp.col = 4;
    m.matrix[12][0] = temp;
    m.matrix[6][3] = temp;
    m.matrix[7][3] = temp;
    m.matrix[11][2] = temp;
    m.matrix[13][1] = temp;
    m.matrix[20][2] = temp;
    m.matrix[21][1] = temp;
    m.matrix[22][1] = temp;
    temp.col = 5;
    m.matrix[13][0] = temp;
    m.matrix[12][4] = temp;
    m.matrix[14][1] = temp;
    m.matrix[22][2] = temp;
    temp.col = 6;
    m.matrix[14][0] = temp;
    m.matrix[13][2] = temp;
    m.matrix[15][1] = temp;
    m.matrix[23][1] = temp;
    temp.col = 7;
    m.matrix[15][0] = temp;
    m.matrix[14][2] = temp;
    m.matrix[16][1] = temp;
    m.matrix[24][1] = temp;
    temp.col = 8;
    m.matrix[16][0] = temp;
    m.matrix[15][2] = temp;
    m.matrix[25][1] = temp;
    temp.row = 5;
    temp.col = 0;
    m.matrix[17][0] = temp;
    m.matrix[8][2] = temp;
    m.matrix[18][2] = temp;
    m.matrix[26][1] = temp;
    temp.col = 1;
    m.matrix[18][0] = temp;
    m.matrix[9][3] = temp;
    m.matrix[17][2] = temp;
    m.matrix[19][2] = temp;
    m.matrix[27][1] = temp;
    temp.col = 2;
    m.matrix[19][0] = temp;
    m.matrix[10][3] = temp;
    m.matrix[18][3] = temp;
    m.matrix[20][3] = temp;
    m.matrix[28][1] = temp;
    temp.col = 3;
    m.matrix[20][0] = temp;
    m.matrix[11][3] = temp;
    m.matrix[12][5] = temp;
    m.matrix[19][3] = temp;
    m.matrix[21][2] = temp;
    m.matrix[28][2] = temp;
    m.matrix[29][1] = temp;
    temp.col = 4;
    m.matrix[21][0] = temp;
    m.matrix[12][6] = temp;
    m.matrix[20][4] = temp;
    m.matrix[22][3] = temp;
    m.matrix[30][1] = temp;
    temp.col = 5;
    m.matrix[22][0] = temp;
    m.matrix[12][7] = temp;
    m.matrix[13][3] = temp;
    m.matrix[21][3] = temp;
    m.matrix[23][2] = temp;
    m.matrix[31][1] = temp;
    m.matrix[32][1] = temp;
    temp.col = 6;
    m.matrix[23][0] = temp;
    m.matrix[14][3] = temp;
    m.matrix[22][4] = temp;
    m.matrix[24][2] = temp;
    m.matrix[32][2] = temp;
    temp.col = 7;
    m.matrix[24][0] = temp;
    m.matrix[15][3] = temp;
    m.matrix[23][3] = temp;
    m.matrix[25][2] = temp;
    m.matrix[33][1] = temp;
    temp.col = 8;
    m.matrix[25][0] = temp;
    m.matrix[16][2] = temp;
    m.matrix[24][3] = temp;
    m.matrix[34][1] = temp;
    temp.row = 6;
    temp.col = 0;
    m.matrix[26][0] = temp;
    m.matrix[17][3] = temp;
    m.matrix[27][2] = temp;
    m.matrix[35][1] = temp;
    temp.col = 1;
    m.matrix[27][0] = temp;
    m.matrix[18][4] = temp;
    m.matrix[26][2] = temp;
    m.matrix[28][3] = temp;
    m.matrix[36][1] = temp;
    temp.col = 2;
    m.matrix[28][0] = temp;
    m.matrix[19][4] = temp;
    m.matrix[20][5] = temp;
    m.matrix[27][3] = temp;
    m.matrix[29][2]= temp;
    m.matrix[26][2] = temp;
    m.matrix[37][1] = temp;
    temp.col = 3;
    m.matrix[29][0] = temp;
    m.matrix[20][6] = temp;
    m.matrix[28][4] = temp;
    m.matrix[30][2] = temp;
    m.matrix[38][1] = temp;
    temp.col = 4;
    m.matrix[30][0] = temp;
    m.matrix[21][4] = temp;
    m.matrix[29][3] = temp;
    m.matrix[31][2] = temp;
    m.matrix[39][1] = temp;
    temp.col = 5;
    m.matrix[31][0] = temp;
    m.matrix[22][5] = temp;
    m.matrix[30][3] = temp;
    m.matrix[32][3] = temp;
    m.matrix[40][1] = temp;
    temp.col = 6;
    m.matrix[32][0] = temp;
    m.matrix[22][6] = temp;
    m.matrix[23][4] = temp;
    m.matrix[31][3] = temp;
    m.matrix[33][2] = temp;
    m.matrix[41][1] = temp;
    m.matrix[42][1] = temp;
    temp.col = 7;
    m.matrix[33][0] = temp;
    m.matrix[24][4] = temp;
    m.matrix[32][4] = temp;
    m.matrix[34][2] = temp;
    m.matrix[42][2] = temp;
    temp.col = 8;
    m.matrix[34][0] = temp;
    m.matrix[25][3] = temp;
    m.matrix[33][3] = temp;
    m.matrix[43][1] = temp;
    temp.row = 7;
    temp.col = 0;
    m.matrix[35][0] = temp;
    m.matrix[26][3] = temp;
    m.matrix[36][3] = temp;
    m.matrix[44][1] = temp;
    temp.col = 1;
    m.matrix[36][0] = temp;
    m.matrix[27][4] = temp;
    m.matrix[28][5] = temp;
    m.matrix[35][2] = temp;
    m.matrix[37][2] = temp;
    m.matrix[44][2] = temp;
    m.matrix[45][1] = temp;
    temp.col = 2;
    m.matrix[37][0] = temp;
    m.matrix[28][6] = temp;
    m.matrix[36][4] = temp;
    m.matrix[38][2] = temp;
    m.matrix[46][1] = temp;
    temp.col = 3;
    m.matrix[38][0] = temp;
    m.matrix[29][4] = temp;
    m.matrix[37][3] = temp;
    m.matrix[39][2] = temp;
    m.matrix[47][1] = temp;
    temp.col = 4;
    m.matrix[39][0] = temp;
    m.matrix[30][4] = temp;
    m.matrix[38][3] = temp;
    m.matrix[40][2] = temp;
    m.matrix[48][1] = temp;
    temp.col = 5;
    m.matrix[40][0] = temp;
    m.matrix[31][4] = temp;
    m.matrix[39][3] = temp;
    m.matrix[41][2] = temp;
    m.matrix[49][1] = temp;
    temp.col = 6;
    m.matrix[41][0] = temp;
    m.matrix[32][5] = temp;
    m.matrix[40][3] = temp;
    m.matrix[42][3] = temp;
    m.matrix[50][1] = temp;
    temp.col = 7;
    m.matrix[42][0] = temp;
    m.matrix[32][6] = temp;
    m.matrix[33][4] = temp;
    m.matrix[41][3] = temp;
    m.matrix[43][2] = temp;
    m.matrix[51][1] = temp;
    m.matrix[52][1] = temp;
    temp.col = 8;
    m.matrix[43][0] = temp;
    m.matrix[34][3] = temp;
    m.matrix[42][4] = temp;
    m.matrix[52][2] = temp;
    temp.row = 8;
    temp.col = 0;
    m.matrix[44][0] = temp;
    m.matrix[35][3] = temp;
    m.matrix[36][5] = temp;
    m.matrix[45][2] = temp;
    m.matrix[53][1] = temp;
    m.matrix[54][1] = temp;
    temp.col = 1;
    m.matrix[45][0] = temp;
    m.matrix[36][6] = temp;
    m.matrix[44][3] = temp;
    m.matrix[46][2] = temp;
    m.matrix[54][2] = temp;
    temp.col = 2;
    m.matrix[46][0] = temp;
    m.matrix[37][4] = temp;
    m.matrix[45][3] = temp;
    m.matrix[47][2] = temp;
    m.matrix[55][1] = temp;
    temp.col = 3;
    m.matrix[47][0] = temp;
    m.matrix[38][4] = temp;
    m.matrix[46][3] = temp;
    m.matrix[48][2] = temp;
    m.matrix[56][1] = temp;
    temp.col = 4;
    m.matrix[48][0] = temp;
    m.matrix[39][4] = temp;
    m.matrix[47][3] = temp;
    m.matrix[49][2] = temp;
    m.matrix[57][1] = temp;
    temp.col = 5;
    m.matrix[49][0] = temp;
    m.matrix[40][4] = temp;
    m.matrix[48][3] = temp;
    m.matrix[50][2] = temp;
    m.matrix[58][1] = temp;
    temp.col = 6;
    m.matrix[50][0] = temp;
    m.matrix[41][4] = temp;
    m.matrix[49][3] = temp;
    m.matrix[51][2] = temp;
    m.matrix[59][1] = temp;
    temp.col = 7;
    m.matrix[51][0] = temp;
    m.matrix[42][4] = temp;
    m.matrix[50][3] = temp;
    m.matrix[52][2] = temp;
    m.matrix[60][1] = temp;
    temp.col = 8;
    m.matrix[52][0] = temp;
    m.matrix[42][6] = temp;
    m.matrix[43][3] = temp;
    m.matrix[51][3] = temp;
    m.matrix[60][2] = temp;
    m.matrix[61][1] = temp;
    temp.row = 9;
    temp.col = 0;
    m.matrix[53][0] = temp;
    m.matrix[44][5] = temp;
    m.matrix[54][3] = temp;
    m.matrix[62][1] = temp;
    temp.col = 1;
    m.matrix[54][0] = temp;
    m.matrix[44][4] = temp;
    m.matrix[45][4] = temp;
    m.matrix[53][2] = temp;
    m.matrix[55][2] = temp;
    m.matrix[63][1] = temp;
    m.matrix[64][1] = temp;
    temp.col = 2;
    m.matrix[55][0] = temp;
    m.matrix[46][4] = temp;
    m.matrix[54][4] = temp;
    m.matrix[56][2] = temp;
    m.matrix[64][2] = temp;
    temp.col = 3;
    m.matrix[56][0] = temp;
    m.matrix[47][4] = temp;
    m.matrix[55][3] = temp;
    m.matrix[57][2] = temp;
    m.matrix[65][1] = temp;
    temp.col = 4;
    m.matrix[57][0] = temp;
    m.matrix[48][4] = temp;
    m.matrix[56][3] = temp;
    m.matrix[58][2] = temp;
    m.matrix[66][1] = temp;
    temp.col = 5;
    m.matrix[58][0] = temp;
    m.matrix[49][4] = temp;
    m.matrix[57][3] = temp;
    m.matrix[59][2] = temp;
    m.matrix[67][1] = temp;
    temp.col = 6;
    m.matrix[59][0] = temp;
    m.matrix[50][4] = temp;
    m.matrix[58][3] = temp;
    m.matrix[60][3] = temp;
    m.matrix[68][1] = temp;
    temp.col = 7;
    m.matrix[60][0] = temp;
    m.matrix[51][4] = temp;
    m.matrix[52][4] = temp;
    m.matrix[59][3] = temp;
    m.matrix[61][2] = temp;
    m.matrix[68][2] = temp;
    m.matrix[69][1] = temp;
    temp.col = 8;
    m.matrix[61][0] = temp;
    m.matrix[52][5] = temp;
    m.matrix[60][4] = temp;
    m.matrix[70][1] = temp;
    temp.row = 10;
    temp.col = 0;
    m.matrix[62][0] = temp;
    m.matrix[53][3] = temp;
    m.matrix[63][2] = temp;
    m.matrix[71][1] = temp;
    temp.col = 1;
    m.matrix[63][0] = temp;
    m.matrix[54][5] = temp;
    m.matrix[62][2] = temp;
    m.matrix[64][3] = temp;
    m.matrix[72][1] = temp;
    temp.col = 2;
    m.matrix[64][0] = temp;
    m.matrix[54][6] = temp;
    m.matrix[55][4] = temp;
    m.matrix[63][3] = temp;
    m.matrix[65][2] = temp;
    m.matrix[73][1] = temp;
    m.matrix[74][1] = temp;
    temp.col = 3;
    m.matrix[65][0] = temp;
    m.matrix[56][4] = temp;
    m.matrix[64][4] = temp;
    m.matrix[66][2] = temp;
    m.matrix[74][2] = temp;
    temp.col = 4;
    m.matrix[66][0] = temp;
    m.matrix[57][4] = temp;
    m.matrix[65][3] = temp;
    m.matrix[67][2] = temp;
    m.matrix[75][1] = temp;
    temp.col = 5;
    m.matrix[67][0] = temp;
    m.matrix[58][4] = temp;
    m.matrix[66][3] = temp;
    m.matrix[68][3] = temp;
    m.matrix[76][1] = temp;
    temp.col = 6;
    m.matrix[68][0] = temp;
    m.matrix[59][4] = temp;
    m.matrix[60][5] = temp;
    m.matrix[67][3] = temp;
    m.matrix[69][3] = temp;
    m.matrix[76][2] = temp;
    m.matrix[77][1] = temp;
    temp.col = 7;
    m.matrix[69][0] = temp;
    m.matrix[60][6] = temp;
    m.matrix[68][4] = temp;
    m.matrix[70][2] = temp;
    m.matrix[78][1] = temp;
    temp.col = 8;
    m.matrix[70][0] = temp;
    m.matrix[61][3] = temp;
    m.matrix[69][3] = temp;
    m.matrix[79][1] = temp;
    temp.row = 11;
    temp.col = 0;
    m.matrix[71][0] = temp;
    m.matrix[62][3] = temp;
    m.matrix[72][2] = temp;
    m.matrix[80][1] = temp;
    temp.col = 1;
    m.matrix[72][0] = temp;
    m.matrix[63][4] = temp;
    m.matrix[71][2] = temp;
    m.matrix[73][2] = temp;
    m.matrix[81][1] = temp;
    temp.col = 2;
    m.matrix[73][0] = temp;
    m.matrix[64][5] = temp;
    m.matrix[72][3] = temp;
    m.matrix[74][3] = temp;
    m.matrix[82][1] = temp;
    temp.col = 3;
    m.matrix[74][0] = temp;
    m.matrix[64][6] = temp;
    m.matrix[65][4] = temp;
    m.matrix[73][3] = temp;
    m.matrix[75][2] = temp;
    m.matrix[83][1] = temp;
    m.matrix[84][1] = temp;
    temp.col = 4;
    m.matrix[75][0] = temp;
    m.matrix[66][4] = temp;
    m.matrix[74][4] = temp;
    m.matrix[76][3] = temp;
    m.matrix[84][2] = temp;
    temp.col = 5;
    m.matrix[76][0] = temp;
    m.matrix[67][4] = temp;
    m.matrix[68][5] = temp;
    m.matrix[75][3] = temp;
    m.matrix[77][2] = temp;
    m.matrix[84][3] = temp;
    m.matrix[85][1] = temp;
    temp.col = 6;
    m.matrix[77][0] = temp;
    m.matrix[86][6] = temp;
    m.matrix[76][4] = temp;
    m.matrix[78][2] = temp;
    m.matrix[86][1] = temp;
    temp.col = 7;
    m.matrix[78][0] = temp;
    m.matrix[69][4] = temp;
    m.matrix[77][3] = temp;
    m.matrix[79][2] = temp;
    m.matrix[87][1] = temp;
    temp.col = 8;
    m.matrix[79][0] = temp;
    m.matrix[70][3] = temp;
    m.matrix[78][3] = temp;
    m.matrix[88][1] = temp;
    temp.row = 12;
    temp.col = 0;
    m.matrix[80][0] = temp;
    m.matrix[71][3] = temp;
    m.matrix[81][2] = temp;
    temp.col = 1;
    m.matrix[81][0] = temp;
    m.matrix[72][4] = temp;
    m.matrix[80][2] = temp;
    m.matrix[82][2] = temp;
    temp.col = 2;
    m.matrix[82][0] = temp;
    m.matrix[73][4] = temp;
    m.matrix[81][3] = temp;
    m.matrix[83][2] = temp;
    temp.col = 3;
    m.matrix[83][0] = temp;
    m.matrix[72][5] = temp;
    m.matrix[82][3] = temp;
    m.matrix[84][4] = temp;
    temp.col = 4;
    m.matrix[84][0] = temp;
    m.matrix[72][6] = temp;
    m.matrix[73][4] = temp;
    m.matrix[74][5] = temp;
    m.matrix[83][3] = temp;
    m.matrix[85][2] = temp;
    temp.col = 5;
    m.matrix[85][0] = temp;
    m.matrix[76][6] = temp;
    m.matrix[84][5] = temp;
    m.matrix[86][2] = temp;
    temp.col = 6;
    m.matrix[86][0] = temp;
    m.matrix[77][4] = temp;
    m.matrix[85][3] = temp;
    m.matrix[87][2] = temp;
    temp.col = 7;
    m.matrix[87][0] = temp;
    m.matrix[78][4] = temp;
    m.matrix[86][3] = temp;
    m.matrix[88][2] = temp;
    temp.col = 8;
    m.matrix[88][0] = temp;
    m.matrix[79][3] = temp;
    m.matrix[87][3] = temp;
    
    m.populated = true;
}

/*
 * Each group will implement the Move function and
 * submit their code using the Upload Site.
 *
 * You should name your move function: Move_GroupName.
 *
 * This will create a function prototype something like:
 *
 *  Move_t Move_GroupName(vector<Token_t>, Color_t turn);
 */

#endif /* TigerBlood_h */
