#ifndef femreader_h__2004_03_07_14_03_begin_guarded_code
#define femreader_h__2004_03_07_14_03_begin_guarded_code

#include <math.h>
#include <fstream>
#include "qwt3d_global.h"
#include "qwt3d_types.h"

class NodeFilter
{
	public:
		explicit NodeFilter()
		{
			values = std::vector<double>(6);
		}
		
		Qwt3D::Triple readLine(std::ifstream& str)
		{
			for (unsigned i = 0; i!=values.size(); ++i)
				str >> values[i];

      return Qwt3D::Triple(values[1], values[2], values[5] / 1000);
		}
	
	private:				
		std::vector<double> values;
};

class CellFilter
{
	public:
		
		Qwt3D::Cell readLine(std::ifstream& str)
		{
			Qwt3D::Cell cell(4);
			str >> cell[0]; // dummy (cell number) - overridden in next step
			for (unsigned i = 0; i<cell.size(); ++i)
			{
				str >> cell[i];
				cell[i] = cell[i] - 1;
			}
			return cell;
		}
};


template <typename FILTER>
bool readNodes(Qwt3D::TripleVector& v, const char* fname, FILTER fil)
{
	std::ifstream file(fname);
	
	v.clear();

	Qwt3D::Triple t;
	while ( file ) 
	{
		t = fil.readLine( file );			
		if (!file.good())
			break;
		v.push_back( t );
	}
	return true;
}

template <typename FILTER>
bool readConnections(Qwt3D::CellVector& v, const char* fname, FILTER fil)
{
	std::ifstream file(fname);

	v.clear();

	Qwt3D::Cell cell;
	while ( file ) 
	{
		cell = fil.readLine( file );
		if (!file.good())
			break;
		v.push_back(cell);	
	}
	return true;
}

#endif /* include guarded */
