enum CellState {unknown, empty, filled};

class Cell
{
private:
	int ColNum;
	int RowNum;
	CellState cellState;
public:
	Cell();
	Cell(int cn, int rn);
	bool Valid() const;
	CellState GetState() const;
	void SetState(const CellState cs);
};