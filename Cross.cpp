#include "Cross.h"
#include <fstream>
#include <string>

Cross::Cross()
{
	//
	CrossWidth = CrossHeight = -1;
}

Cross::Cross(const char* FileName)
{
	GetFromFile(FileName);
}

void Cross::GetFromFile(const char* FileName)
{
	Table.clear();
	Rows.clear();
	Cols.clear();
	RowGroups.clear();
	ColGroups.clear();
	ifstream InputFile(FileName, ios::in);
	InputFile >> CrossHeight >> CrossWidth;
	Rows.resize(CrossHeight);
	Cols.resize(CrossWidth);

	RowGroups.resize(CrossHeight);
	ColGroups.resize(CrossWidth);

	int RowCount = 0, ColCount = 0;

	array<int, 2> tmp;
	tmp[0] = 0;
	tmp[1] = CrossWidth;

	for (int i = 0; i<CrossHeight; i++)
	{
		int count;
		InputFile >> count;
		Rows[i].resize(count);
		RowGroups[i].push_back(tmp);
		for (int j = 0; j<count; j++)
		{
			InputFile >> Rows[i][j];
			RowCount+=Rows[i][j];
		}
	}

	tmp[0] = 0;
	tmp[1] = CrossHeight;

	for (int i = 0; i<CrossWidth; i++)
	{
		int count;
		InputFile >> count;
		Cols[i].resize(count);
		ColGroups[i].push_back(tmp);
		for (int j = 0; j<count; j++)
		{
			InputFile >> Cols[i][j];
			ColCount+=Cols[i][j];
		}
	}

	Table.resize(CrossHeight);
	for (int i = 0; i<CrossHeight; i++)
		Table[i].resize(CrossWidth);
	cout << RowCount << ' ' << ColCount << endl;
}

void Cross::LeftmostRow(const int index, vector<int>& Res, int CurBlock, int CurPos, int CurGroup, bool& can)
{
	if (Rows[index].empty())
		return;
	
	can = false;
	while (!can)
	{
		can = true;

		if (Res.size()==Rows[index].size())
		{
			bool b = true;
			for (int i = Res.back()+Rows[index][CurBlock-1]; i<CrossWidth && b; i++)
				b = (Table[index][i].GetState()!=CellState::filled);
			if (!b)
				can = false;
			return;
		}

		if (CurPos>=CrossWidth)
		{
			can = false;
			return;
		}
		bool b = true;
		for (int i = ((!Res.empty()) ? (Res.back()+Rows[index][CurBlock-1]) : 0); i<CurPos && b; i++)
			b = (Table[index][i].GetState()!=CellState::filled);
		if (!b)
		{
			can = false;
			return;
		}

		if (Table[index][CurPos].GetState()==CellState::empty)
		{
			if (CurPos!=0)
				CurGroup++;
			if (CurGroup==RowGroups[index].size())
			{
				can = false;
				return;
			}
			CurPos = RowGroups[index][CurGroup][0];
		}

		if (RowGroups[index][CurGroup][0]+RowGroups[index][CurGroup][1]-CurPos<Rows[index][CurBlock])
		{
			bool b = true;
			for (int i = CurPos; i<RowGroups[index][CurGroup][0]+RowGroups[index][CurGroup][1] && b; i++)
				b = (Table[index][i].GetState()!=CellState::filled);
			if (b)
			{
				CurGroup++;
				if (CurGroup==RowGroups[index].size())
				{
					can = false;
					return;
				}
				CurPos = RowGroups[index][CurGroup][0];
				can = false;
				continue;
			} else
			{
				can = false;
				return;
			}
		}
	
		if (CurPos+Rows[index][CurBlock]>CrossWidth)
		{
			can = false;
			return;
		}

		if (CurPos+Rows[index][CurBlock]==CrossWidth)
		{
			CurBlock++;
			if (Rows[index].size()==CurBlock)
			{
				Res.push_back(CurPos);
				return;
			} else
			{
				can = false;
				return;
			}
		}

		if (Table[index][CurPos].GetState()==CellState::filled && Table[index][CurPos+Rows[index][CurBlock]].GetState()==CellState::filled)
		{
			can = false;
			return;
		}


		switch (Table[index][CurPos+Rows[index][CurBlock]].GetState())
		{
		case CellState::empty:
			Res.push_back(CurPos);
			CurPos += (Rows[index][CurBlock]+1);
			CurBlock++;
			LeftmostRow(index, Res, CurBlock, CurPos, CurGroup, can);
			if (!can)
			{
				CurPos = Res.back()+1;
				for (int i = 0; i<RowGroups[index].size(); i++)
					if (CurPos>=RowGroups[index][i][0] && CurPos<RowGroups[index][i][0]+RowGroups[index][i][1])
					{
						CurGroup = i;
						break;
					}
				CurBlock--;
				Res.pop_back();
			}
			break;
		case CellState::unknown:
			Res.push_back(CurPos);
			CurPos += (Rows[index][CurBlock]+1);
			CurBlock++;
			LeftmostRow(index, Res, CurBlock, CurPos, CurGroup, can);
			if (!can)
			{
				CurPos = Res.back()+1;
				for (int i = 0; i<RowGroups[index].size(); i++)
					if (CurPos>=RowGroups[index][i][0] && CurPos<RowGroups[index][i][0]+RowGroups[index][i][1])
					{
						CurGroup = i;
						break;
					}
				CurBlock--;
				Res.pop_back();
			}
			break;
		case CellState::filled:
			CurPos++;
			can = false;
			break;
		}
		if (!can)
		{
			if (CurPos>=CrossWidth)
				return;
		}
	}
	return;
}

vector<int> Cross::RightmostRow(const int index)
{
	vector<int> Res;
	bool b = true;

	reverse(Table[index].begin(), Table[index].end());
	reverse(Rows[index].begin(), Rows[index].end());
	for (int i = 0; i<RowGroups[index].size(); i++)
		RowGroups[index][i][0] = CrossWidth-(RowGroups[index][i][0]+RowGroups[index][i][1]);
	reverse(RowGroups[index].begin(), RowGroups[index].end());

	LeftmostRow(index, Res, 0, 0, 0, b);

	for (int i = 0; i<RowGroups[index].size(); i++)
		RowGroups[index][i][0] = CrossWidth-(RowGroups[index][i][0]+RowGroups[index][i][1]);
	reverse(RowGroups[index].begin(), RowGroups[index].end());

	reverse(Rows[index].begin(), Rows[index].end());
	reverse(Table[index].begin(), Table[index].end());
	
	reverse(Res.begin(), Res.end());
	
	for (int i = 0; i<Res.size(); i++)
		Res[i] = CrossWidth-Res[i]-Rows[index][i];
		
	return Res;
}

void Cross::UppermostCol(const int index, vector<int>& Res, int CurBlock, int CurPos, int CurGroup, bool& can)
{
	if (Cols[index].empty())
		return;
	
	can = false;
	while (!can)
	{
		can = true;

		if (Res.size()==Cols[index].size())
		{
			bool b = true;
			for (int i = Res.back()+Cols[index][CurBlock-1]; i<CrossHeight && b; i++)
				b = (Table[i][index].GetState()!=CellState::filled);
			if (!b)
				can = false;
			return;
		}

		if (CurPos>=CrossHeight)
		{
			can = false;
			return;
		}
		
		bool b = true;
		for (int i = ((!Res.empty()) ? (Res.back()+Cols[index][CurBlock-1]) : 0); i<CurPos && b; i++)
			b = (Table[i][index].GetState()!=CellState::filled);
		if (!b)
		{
			can = false;
			return;
		}

		if (Table[CurPos][index].GetState()==CellState::empty)
		{
			if (CurPos!=0)
				CurGroup++;
			if (CurGroup==ColGroups[index].size())
			{
				can = false;
				return;
			}
			CurPos = ColGroups[index][CurGroup][0];
		}

		if (ColGroups[index][CurGroup][0]+ColGroups[index][CurGroup][1]-CurPos<Cols[index][CurBlock])
		{
			bool b = true;
			for (int i = CurPos; i<ColGroups[index][CurGroup][0]+ColGroups[index][CurGroup][1] && b; i++)
				b = (Table[i][index].GetState()!=CellState::filled);
			if (b)
			{
				CurGroup++;
				if (CurGroup==ColGroups[index].size())
				{
					can = false;
					return;
				}
				CurPos = ColGroups[index][CurGroup][0];
				can = false;
				continue;
			} else
			{
				can = false;
				return;
			}
		}
	
		if (CurPos+Cols[index][CurBlock]>CrossHeight)
		{
			can = false;
			return;
		}

		if (CurPos+Cols[index][CurBlock]==CrossHeight)
		{
			CurBlock++;
			if (Cols[index].size()==CurBlock)
			{
				Res.push_back(CurPos);
				return;
			} else
			{
				can = false;
				return;
			}
		}

		if (Table[CurPos][index].GetState()==CellState::filled && Table[CurPos+Cols[index][CurBlock]][index].GetState()==CellState::filled)
		{
			can = false;
			return;
		}


		switch (Table[CurPos+Cols[index][CurBlock]][index].GetState())
		{
		case CellState::empty:
			Res.push_back(CurPos);
			CurPos += (Cols[index][CurBlock]+1);
			CurBlock++;
			UppermostCol(index, Res, CurBlock, CurPos, CurGroup, can);
			if (!can)
			{
				CurPos = Res.back()+1;
				for (int i = 0; i<ColGroups[index].size(); i++)
					if (CurPos>=ColGroups[index][i][0] && CurPos<ColGroups[index][i][0]+ColGroups[index][i][1])
					{
						CurGroup = i;
						break;
					}
				CurBlock--;
				Res.pop_back();
			}
			break;
		case CellState::unknown:
			Res.push_back(CurPos);
			CurPos += (Cols[index][CurBlock]+1);
			CurBlock++;
			UppermostCol(index, Res, CurBlock, CurPos, CurGroup, can);
			if (!can)
			{
				CurPos = Res.back()+1;
				for (int i = 0; i<ColGroups[index].size(); i++)
					if (CurPos>=ColGroups[index][i][0] && CurPos<ColGroups[index][i][0]+ColGroups[index][i][1])
					{
						CurGroup = i;
						break;
					}
				CurBlock--;
				Res.pop_back();
			}
			break;
		case CellState::filled:
			CurPos++;
			can = false;
			break;
		}
		if (!can)
		{
			if (CurPos>=CrossHeight)
				return;
		}
	}
	return;
}

vector<int> Cross::LowermostCol(const int index)
{
	vector<int> Res;
	
	for (int i = 0; i<Table.size()/2; i++)
		swap(Table[i][index], Table[Table.size()-1-i][index]);
	reverse(Cols[index].begin(), Cols[index].end());
	for (int i = 0; i<ColGroups[index].size(); i++)
		ColGroups[index][i][0] = CrossHeight-(ColGroups[index][i][0]+ColGroups[index][i][1]);
	reverse(ColGroups[index].begin(), ColGroups[index].end());

	bool b = true;
	UppermostCol(index, Res, 0, 0, 0, b);

	for (int i = 0; i<ColGroups[index].size(); i++)
		ColGroups[index][i][0] = CrossHeight-(ColGroups[index][i][0]+ColGroups[index][i][1]);
	reverse(ColGroups[index].begin(), ColGroups[index].end());
	reverse(Cols[index].begin(), Cols[index].end());
	for (int i = 0; i<Table.size()/2; i++)
		swap(Table[i][index], Table[Table.size()-1-i][index]);
	
	reverse(Res.begin(), Res.end());
	
	for (int i = 0; i<Res.size(); i++)
		Res[i] = CrossHeight-Res[i]-Cols[index][i];
	return Res;
}

void Cross::Solve()
{
	bool Changed = true;
	
	auto Regroup = [this](int Row, int Col, CellState c)
	{
		Table[Row][Col].SetState(c);
		if (c==CellState::filled)
			return;

		for (int k = 0; k<RowGroups[Row].size(); k++)
		{
			if (Col>=RowGroups[Row][k][0] && Col<RowGroups[Row][k][0]+RowGroups[Row][k][1])
			{
				if (RowGroups[Row][k][1]==1)
				{
					RowGroups[Row].erase(RowGroups[Row].begin()+k);
				}
				else if (RowGroups[Row][k][0]==Col)
				{
					RowGroups[Row][k][0]++;
					RowGroups[Row][k][1]--;
				}
				else if (RowGroups[Row][k][0]+RowGroups[Row][k][1]-1==Col)
				{
					RowGroups[Row][k][1]--;
				}
				else
				{
					array<int, 2> tmp;
					tmp[0] = Col+1;
					tmp[1] = RowGroups[Row][k][0]+RowGroups[Row][k][1]-1-Col;
					RowGroups[Row][k][1] = Col-RowGroups[Row][k][0];
					RowGroups[Row].insert(RowGroups[Row].begin()+k+1, tmp);
				}
				break;
			}
		}

		for (int k = 0; k<ColGroups[Col].size(); k++)
		{
			if (Row>=ColGroups[Col][k][0] && Row<ColGroups[Col][k][0]+ColGroups[Col][k][1])
			{
				if (ColGroups[Col][k][1]==1)
				{
					ColGroups[Col].erase(ColGroups[Col].begin()+k);
				}
				else if (ColGroups[Col][k][0]==Row)
				{
					ColGroups[Col][k][0]++;
					ColGroups[Col][k][1]--;
				}
				else if (ColGroups[Col][k][0]+ColGroups[Col][k][1]-1==Row)
				{
					ColGroups[Col][k][1]--;
				}
				else
				{
					array<int, 2> tmp;
					tmp[0] = Row+1;
					tmp[1] = ColGroups[Col][k][0]+ColGroups[Col][k][1]-1-Row;
					ColGroups[Col][k][1] = Row-ColGroups[Col][k][0];
					ColGroups[Col].insert(ColGroups[Col].begin()+k+1, tmp);
				}
				break;
			}
		}
	};

	auto IntToStr = [](int Num)->string
	{
		if (Num==0)
			return "0";
		string Res = "";
		while (Num)
		{
			char c = '0'+(Num%10);
			Res = c+Res;
			Num /= 10;
		}
		return Res;
	};

	int it = 0;
	string OutputName = "";

	while (Changed)
	{
		Changed = false;
		for (int i = 0; i<CrossHeight; i++)
		{
			vector<int> Leftmost;
			bool b = true;
 			LeftmostRow(i, Leftmost, 0, 0, 0, b);
			vector<int> Rightmost = RightmostRow(i);
			for (int j = 0; j<Rows[i].size(); j++)
			{
				for (int k = Rightmost[j]; k<Leftmost[j]+Rows[i][j]; k++)
				{
					Changed |= (Table[i][k].GetState()!=CellState::filled);
					Table[i][k].SetState(CellState::filled);
				}

				if (Leftmost[j]==Rightmost[j])
				{
					if (Leftmost[j]!=0)
					{
						Changed |= (Table[i][Leftmost[j]-1].GetState()!=CellState::empty);
						Table[i][Leftmost[j]-1].SetState(CellState::empty);
						Regroup(i, Leftmost[j]-1, CellState::empty);
					}
					if (Leftmost[j]+Rows[i][j]!=CrossWidth)
					{
						Changed |= (Table[i][Leftmost[j]+Rows[i][j]].GetState()!=CellState::empty);
						Table[i][Leftmost[j]+Rows[i][j]].SetState(CellState::empty);
						Regroup(i, Leftmost[j]+Rows[i][j], CellState::empty);
					}
				}
			}
			for (int k = 0; k<Leftmost[0]; k++)
			{
				Changed |= (Table[i][k].GetState()!=CellState::empty);
				Table[i][k].SetState(CellState::empty);
				Regroup(i, k, CellState::empty);
			}
			for (int j = 0; j<Rows[i].size()-1; j++)
			{
				for (int k = Rightmost[j]+Rows[i][j]; k<Leftmost[j+1]; k++)
				{
					Changed |= (Table[i][k].GetState()!=CellState::empty);
					Table[i][k].SetState(CellState::empty);
					Regroup(i, k, CellState::empty);
				}
			}
			for (int k = Rightmost.back()+Rows[i].back(); k<CrossWidth; k++)
			{
				Changed |= (Table[i][k].GetState()!=CellState::empty);
				Table[i][k].SetState(CellState::empty);
				Regroup(i, k, CellState::empty);
			}
		}

		OutputName = IntToStr(it)+".txt";
		DrawToFile(OutputName);
		it++;

		for (int i = 0; i<CrossWidth; i++)
		{
			vector<int> Uppermost;
			bool b = true;
			UppermostCol(i, Uppermost, 0, 0, 0, b);
			vector<int> Lowermost = LowermostCol(i);
			for (int j = 0; j<Cols[i].size(); j++)
			{
				for (int k = Lowermost[j]; k<Uppermost[j]+Cols[i][j]; k++)
				{
					Changed |= (Table[k][i].GetState()!=CellState::filled);
					Table[k][i].SetState(CellState::filled);
				}

				if (Uppermost[j]==Lowermost[j])
				{
					if (Uppermost[j]!=0)
					{
						Changed |= (Table[Uppermost[j]-1][i].GetState()!=CellState::empty);
						Table[Uppermost[j]-1][i].SetState(CellState::empty);
						Regroup(Uppermost[j]-1, i, CellState::empty);
					}
					if (Uppermost[j]+Cols[i][j]!=CrossHeight)
					{
						Changed |= (Table[Uppermost[j]+Cols[i][j]][i].GetState()!=CellState::empty);
						Table[Uppermost[j]+Cols[i][j]][i].SetState(CellState::empty);
						Regroup(Uppermost[j]+Cols[i][j], i, CellState::empty);
					}
				}
			}

			for (int k = 0; k<Uppermost[0]; k++)
			{
				Changed |= (Table[k][i].GetState()!=CellState::empty);
				Table[k][i].SetState(CellState::empty);
				Regroup(k, i, CellState::empty);
			}
			for (int j = 0; j<Cols[i].size()-1; j++)
			{
				for (int k = Lowermost[j]+Cols[i][j]; k<Uppermost[j+1]; k++)
				{
					Changed |= (Table[k][i].GetState()!=CellState::empty);
					Table[k][i].SetState(CellState::empty);
					Regroup(k, i, CellState::empty);
				}
			}
			for (int k = Lowermost.back()+Cols[i].back(); k<CrossHeight; k++)
			{
				Changed |= (Table[k][i].GetState()!=CellState::empty);
				Table[k][i].SetState(CellState::empty);
				Regroup(k, i, CellState::empty);
			}
		}
		OutputName = IntToStr(it)+".txt";
		DrawToFile(OutputName);
		it++;
		
		if (!Changed)
		{
			for (int i = 0; i<CrossHeight; i++)
				for (int j = 0; j<CrossWidth; j++)
				{
					if (Table[i][j].GetState() == CellState::unknown)
					{
						vector<vector<array<int, 2>>> tmpRowGroups = RowGroups;
						vector<vector<array<int, 2>>> tmpColGroups = ColGroups;
						Regroup(i, j, CellState::empty);
						vector<int> Line;
						bool b = true;
						LeftmostRow(i, Line, 0, 0, 0, b);
						if (!b)
						{
							RowGroups = tmpRowGroups;
							ColGroups = tmpColGroups;
							Regroup(i, j, CellState::filled);
							Changed = true;
							continue;
						}
						Line.clear();
						UppermostCol(j, Line, 0, 0, 0, b);
						if (!b)
						{
							RowGroups = tmpRowGroups;
							ColGroups = tmpColGroups;
							Regroup(i, j, CellState::filled);
							Changed = true;
							continue; 
						}
						Line.clear();

						RowGroups = tmpRowGroups;
						ColGroups = tmpColGroups;

						Table[i][j].SetState(CellState::filled);
						LeftmostRow(i, Line, 0, 0, 0, b);
						if (!b)
						{
							Regroup(i, j, CellState::empty);
							Changed = true;
							continue;
						}
						Line.clear();
						UppermostCol(j, Line, 0, 0, 0, b);
						if (!b)
						{
							Regroup(i, j, CellState::empty);
							Changed = true;
							continue; 
						}
						Table[i][j].SetState(CellState::unknown);
					}
				}
		}
	}
}

void Cross::Draw() const
{
	for (int i = 0; i<CrossHeight; i++)
	{
		for (int j = 0; j<CrossWidth; j++)
			cout << (Table[i][j].GetState()==CellState::filled ? 'o' : (Table[i][j].GetState()==CellState::empty ? ' ' : ' '));
		cout << endl;
	}
}

void Cross::DrawToFile(string FileName) const
{
	ofstream OutFile(FileName);
	for (int i = 0; i<CrossHeight; i++)
	{
		for (int j = 0; j<CrossWidth; j++)
		{
			CellState cs = Table[i][j].GetState();
			char c = ((cs==CellState::filled) ? 'o' : ((cs==CellState::empty) ? '_' : ' '));
			OutFile << c;
		}
		OutFile << endl;
	}
	OutFile.close();
}