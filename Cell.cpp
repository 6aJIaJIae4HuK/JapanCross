#include "Cell.h"

Cell::Cell()
{
	ColNum = -1;
	RowNum = -1;
	cellState = unknown;
}

Cell::Cell(int cn, int rn)
{
	ColNum = cn;
	RowNum = rn;
	cellState = unknown;
}

bool Cell::Valid() const
{
	return (ColNum!=-1 && RowNum!=-1);
}

CellState Cell::GetState() const
{
	return cellState;
}

void Cell::SetState(const CellState cs)
{
	cellState = cs;
}
