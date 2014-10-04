#include <iostream>
#include <list>
#include <vector>
#include <array>
#include "Cell.h"

using namespace std;

class Cross
{
private:
	vector< vector < Cell > > Table;
	vector< vector < int > > Rows, Cols;
	vector< vector < array <int, 2 > > > RowGroups;//array[0] - Index start of group; array[1] - Length of group
	vector< vector < array <int, 2 > > > ColGroups;
	int CrossWidth, CrossHeight;
	void LeftmostRow(const int index, vector<int>& Res, int CurBlock, int CurPos, int CurGroup, bool& can);
	vector<int> RightmostRow(const int index);
	void UppermostCol(const int index, vector<int>& Res, int CurBlock, int CurPos, int CurGroup, bool& can);
	vector<int> LowermostCol(const int index);
public:
	Cross();
	Cross(const char* FileName);
	void GetFromFile(const char* FileName);
	void Solve();
	void Draw() const;
	void DrawToFile(string FileName) const;
};