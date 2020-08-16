#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>
#include <map>

constexpr int nSize = 10; //number of tiles per side
const int nTileSize = 8; //number of pixels per tile_side
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

		for (int i = 0; i < nCellNumber; i++) {
			Cell* candidate = &CellList[i];

			bool north_neighbour = ((*candidate).cy != 0) && (CellList[i - (nSize+1)].CellExists);
			bool west_neighbour = ((*candidate).cx != 0) && (CellList[i - 1].CellExists);
			bool self_exist = (*candidate).CellExists;

			if ((*candidate).cx == 5 && (*candidate).cy == 2) { 
				std::cout << "ouch!"; }
			/*there are four possibilities for a northern edge to exist:
				1: !north !west exist
				2: !north west exist
				3. north !west !exist
				4. north west !exist

			ie:
				bool edge_exist = exist && !north || !exist && north

			for a western edge to exist:
				bool edge_exist = exist && !west || !exist &&  west
			*/

			if ((north_neighbour && !self_exist) || (!north_neighbour && self_exist)) {
				(*candidate).EdgeExists[0] = true;
				//now check if its a new edge or a continuation of an old edge
				if ( ( (*candidate).cx != 0) && (CellList[i - 1].CellExists) && CellList[i-1].EdgeExists[0]) {
					//Continue existing edge - get edge ID and edit endpoint
					int neighbour_ID = CellList[i - 1].EdgeIDs[0];
					Cell::EdgePool[neighbour_ID][2] += 1;
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
			if ((west_neighbour && !self_exist) || (!west_neighbour && self_exist)) {
				(*candidate).EdgeExists[3] = true;
				if (((*candidate).cy != 0) && (CellList[i - (nSize + 1)].CellExists) && CellList[i - (nSize + 1)].EdgeExists[3]) {
					//continuation of above edge - get edge ID and edit endpoint
					int neighbour_ID = CellList[i - (nSize + 1)].EdgeIDs[3];
					Cell::EdgePool[neighbour_ID][3] += 1;
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
		}

		//add 4 borders of screen
		std::vector<int> border_vec;
		int left_border[] = { 0, 0, 0, nSize };
		int top_border[] = { 0, 0, nSize, 0 };
		int right_border[] = { nSize, 0, nSize, nSize };
		int bottom_border[] = { 0, nSize, nSize, nSize };

		border_vec = std::vector<int>(left_border, left_border + sizeof(left_border) / sizeof(int));
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
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		//build array 
		bool texists;
		int tx, ty;
		srand(0);
		for (int i = 0; i < (nSize+1)*(nSize+1); i++) {
			//texists = ((rand() % 256) < 32) ? true : false;
			if ((24 < i && i < 27) || (34 < i && i < 36)) {
				texists = true;
			}
			else {
				texists = false;
			}
			int tx = i % (nSize+1);
			int ty = i / (nSize+1);
			CellList.push_back(Cell(texists, tx, ty));
		}

		//do polygonization step
		ConvertTilesToPolygons();

		//prints current edge list:
		std::cout << "EDGE LIST: " << std::endl;
		for (int n = 0; n < Cell::EdgePool.size(); n++){
			std::cout << std::endl;
			std::cout << Cell::EdgePool[n][0] << ", " << Cell::EdgePool[n][1] << " --> " << Cell::EdgePool[n][2] << ", " << Cell::EdgePool[n][3];	
		}
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//olc::Pixel col(rand() % 256, rand() % 256, rand() % 256);
		olc::Pixel col = olc::DARK_BLUE;
		// called once per frame, draws random coloured pixels
		for (int i = 0; i < nCellNumber; i++) {
			Cell* candidate = &CellList[i];
			if ((*candidate).CellExists){
				for (int jx = 0; jx < nTileSize; jx++) {
					for (int jy = 0; jy < nTileSize; jy++) {
						Draw((*candidate).cx*nTileSize + jx, (*candidate).cy*nTileSize + jy, col );
					}
				}
				
			}
		}
	
		for (int ix = 0; ix < nSize * nTileSize; ix++) {
			for (int iy = 0; iy < nSize * nTileSize; iy++) {
				if (!(ix% nTileSize) || !(iy% nTileSize)) {
					Draw(ix, iy);

				}
			}
		}
		std::map<int, std::vector<int>>::iterator it;
		for (it = Cell::EdgePool.begin(); it != Cell::EdgePool.end(); it++) {
			std::vector<int> line_coords = it->second;
			DrawLine(line_coords[0]*nTileSize, line_coords[1]* nTileSize, line_coords[2]* nTileSize, line_coords[3]* nTileSize, olc::RED);
		}
		return true;
	}
};

int main()
{
	Tiler app;
	if (app.Construct(nSize*nTileSize+1, nSize*nTileSize+1, nTileSize, nTileSize))
		app.Start();
	return 0;
}