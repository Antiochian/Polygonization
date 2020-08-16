#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>
#include <map>

constexpr int nSize = 10;
const int nCellNumber = nSize*nSize; //hardcoded until i learn how this shit works lol

class Cell {
public:
	static std::map<int, std::vector<int>> EdgePool; //dictionary with ID key, format of { [x1s,y1s,x1f,y1f], [x2s,y2s,x2f,y2f], etc}
	static int nextID;

	bool CellExists;
	int cx, cy;
	bool EdgeExists[4]; //N,E,S,W
	int EdgeIDs[4] = { false, false, false, false };

	Cell(bool exists, int x, int y) {
		CellExists = exists;
		cx = x;
		cy = y;
		//constructor
	}
};

std::map<int, std::vector<int>> Cell::EdgePool;
int nextID = 0;
class Tiler : public olc::PixelGameEngine
{
public:
	Tiler() //constructor
	{
		// Name you application
		sAppName = "Tiler";
	}

public:
	std::vector<Cell> CellList;
	void ConvertTilesToPolygons() {
		//add 4 borders of screen
		//Cell::EdgePool
		std::vector<int> border_vec;
		int left_border[] = { 0, 0, 0, nSize };
		int top_border[] = { 0, 0, nSize, 0 };
		int right_border[] = { nSize, 0, nSize, nSize };
		int bottom_border[] = { 0, nSize, nSize, nSize };

		border_vec = std::vector<int> (left_border, left_border + sizeof(left_border) / sizeof(int));
		Cell::EdgePool.insert(std::pair<int, std::vector<int>>(nextID, border_vec));
		nextID += 1;
		border_vec = std::vector<int>(right_border, right_border + sizeof(right_border) / sizeof(int));
		Cell::EdgePool.insert(std::pair<int, std::vector<int>>(nextID, border_vec));
		nextID += 1;
		border_vec = std::vector<int>(top_border, top_border + sizeof(top_border) / sizeof(int));
		Cell::EdgePool.insert(std::pair<int, std::vector<int>>(nextID, border_vec));
		nextID += 1;
		border_vec = std::vector<int>(bottom_border, bottom_border + sizeof(bottom_border) / sizeof(int));
		Cell::EdgePool.insert(std::pair<int, std::vector<int>>(nextID, border_vec));
		nextID += 1;

		for (int i = 0; i < nCellNumber; i++) {
			Cell* candidate = &CellList[i];
			if ((*candidate).CellExists) {
				//check if there is NO northern neighbour
				if (((*candidate).cy != 0) && (!CellList[i - 10].CellExists)) {
					(*candidate).EdgeExists[0] = true;
					//now check if its a new edge or a continuation of an old edge
					if ( ((*candidate).cx != 0) && (CellList[i - 1].CellExists) && CellList[i-1].EdgeExists[0]) {
						//Continue existing edge - get edge ID and edit endpoint
						int neighbour_ID = CellList[i - 1].EdgeIDs[0];
						Cell::EdgePool[neighbour_ID][2] = (*candidate).cx+1;
						Cell::EdgePool[neighbour_ID][3] = (*candidate).cy;
						(*candidate).EdgeIDs[0] = neighbour_ID; //update current edge id to match

					}
					else {
						//no continuation, add new edge to EDGE POOL
						int edge_iter[] = { (*candidate).cx, (*candidate).cy, (*candidate).cx + 1, (*candidate).cy };
						std::vector<int> new_vec(edge_iter, edge_iter + sizeof(edge_iter) / sizeof(int));
						Cell::EdgePool.insert(std::pair<int, std::vector<int>>(nextID, new_vec));
						//advance ID number
						(*candidate).EdgeIDs[0] = nextID;
						nextID += 1;
					}
				}
				//check if there is NO western neighbour (all as before)
				if (((*candidate).cx != 0) && (!CellList[i - 1].CellExists)) {
					(*candidate).EdgeExists[3] = nextID;
					if (((*candidate).cy != 0) && (CellList[i - 10].CellExists) && CellList[i - 10].EdgeExists[3]) {
						//continuation of above edge - get edge ID and edit endpoint
						int neighbour_ID = CellList[i - 10].EdgeIDs[3];
						Cell::EdgePool[neighbour_ID][2] = (*candidate).cx;
						Cell::EdgePool[neighbour_ID][3] = (*candidate).cy + 1;
						(*candidate).EdgeIDs[3] = neighbour_ID; //update current edge id to match
					}
					else {
						//no continuation, add new edge to EDGE POOL
						int edge_iter[] = { (*candidate).cx, (*candidate).cy, (*candidate).cx, (*candidate).cy+1};
						std::vector<int> new_vec(edge_iter, edge_iter + sizeof(edge_iter) / sizeof(int));
						Cell::EdgePool.insert(std::pair<int, std::vector<int>>(nextID, new_vec));
						(*candidate).EdgeIDs[3] = nextID;
						//advance ID number
						nextID += 1;
					}
				}
				//check if there is NO eastern neighbour (all as before)
				if (((*candidate).cx != nSize-1) && (!CellList[i + 1].CellExists)) {
					(*candidate).EdgeExists[1] = nextID;
					if (((*candidate).cy != 0) && (CellList[i - 10].CellExists) && CellList[i - 10].EdgeExists[1]) {
						//continuation of above edge - get edge ID and edit endpoint
						int neighbour_ID = CellList[i - 10].EdgeIDs[3];
						Cell::EdgePool[neighbour_ID][2] = (*candidate).cx+1;
						Cell::EdgePool[neighbour_ID][3] = (*candidate).cy + 1;
						(*candidate).EdgeIDs[1] = neighbour_ID; //update current edge id to match
					}
					else {
						//no continuation, add new edge to EDGE POOL
						int edge_iter[] = { (*candidate).cx+1, (*candidate).cy, (*candidate).cx+1, (*candidate).cy + 1 };
						std::vector<int> new_vec(edge_iter, edge_iter + sizeof(edge_iter) / sizeof(int));
						Cell::EdgePool.insert(std::pair<int, std::vector<int>>(nextID, new_vec));
						(*candidate).EdgeIDs[1] = nextID;
						//advance ID number
						nextID += 1;
					}
				}
				//check if there is NO southern neighbour
				if (((*candidate).cy != nSize-1) && (!CellList[i + 10].CellExists)) {
					(*candidate).EdgeExists[2] = true;
					//now check if its a new edge or a continuation of an old edge
					if (((*candidate).cx != 0) && (CellList[i - 1].CellExists) && CellList[i - 1].EdgeExists[2]) {
						//Continue existing edge - get edge ID and edit endpoint
						int neighbour_ID = CellList[i - 1].EdgeIDs[2];
						Cell::EdgePool[neighbour_ID][2] = (*candidate).cx + 1;
						Cell::EdgePool[neighbour_ID][3] = (*candidate).cy+1;
						(*candidate).EdgeIDs[2] = neighbour_ID; //update current edge id to match

					}
					else {
						//no continuation, add new edge to EDGE POOL
						int edge_iter[] = { (*candidate).cx, (*candidate).cy+1, (*candidate).cx + 1, (*candidate).cy+1 };
						std::vector<int> new_vec(edge_iter, edge_iter + sizeof(edge_iter) / sizeof(int));
						Cell::EdgePool.insert(std::pair<int, std::vector<int>>(nextID, new_vec));
						//advance ID number
						(*candidate).EdgeIDs[2] = nextID;
						nextID += 1;
					}
				}
				
			}
		}
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		//build array 
		bool texists;
		int tx, ty;
		srand(0);
		for (int i = 0; i < nCellNumber; i++) {
			//texists = ((rand() % 256) < 32) ? true : false;
			if ((24 < i && i < 27) || (34 < i && i < 36)) {
				texists = true;
			}
			else {
				texists = false;
			}
			int tx = i % 10;
			int ty = i / 10;
			CellList.push_back(Cell(texists, tx, ty));
		}

		//do polygonization step
		ConvertTilesToPolygons();

		//prints current edge list:
		std::cout << "EDGE LIST: " << std::endl;
		for (int n = 0; n < Cell::EdgePool.size(); n++){
			std::cout << std::endl;
			std::cout << Cell::EdgePool[n][0] << ", " << Cell::EdgePool[n][1] << " -->" << Cell::EdgePool[n][2] << ", " << Cell::EdgePool[n][3];	
		}
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame, draws random coloured pixels
		for (int i = 0; i < nCellNumber; i++) {
			Cell* candidate = &CellList[i];
			if ((*candidate).CellExists){
				Draw((*candidate).cx, (*candidate).cy, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));
			}
		}
		
		return true;
	}
};

int main()
{
	Tiler app;
	if (app.Construct(nSize, nSize, 40, 40))
		app.Start();
	return 0;
}