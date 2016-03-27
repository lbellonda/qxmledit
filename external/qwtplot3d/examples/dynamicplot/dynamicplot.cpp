//-----------------------------------------------------------------
//              dynamicplot.cpp
//
//      A simple example which shows how to use GridPlot
//-----------------------------------------------------------------

#include <math.h>
#include <limits>

#include <QApplication>
#include <QElapsedTimer>

#include "dynamicplot.h"

Plot::Plot()
{
	//setTitle("Dynamic Plotter Demonstration");

    setRenderThreadsCount(1);
    enableFastNormals(true);

	// fill initial data
	for (int i = 0; i < Width1; i++)
	{
		data[i] = new double[Width2];

		double xval = 50;

		for (int j = 0; j < Width2; j++)
		{
			data[i][j] = xval;

			xval += rand() % 3 - 1;
		}
	}

	setRotation(50, 0, -15);
	setZoom(1);
	setPlotStyle(Qwt3D::FILLED);

	for (unsigned i=0; i!=coordinates()->axes.size(); ++i)
	{
		coordinates()->axes[i].setMajors(10);
		coordinates()->axes[i].setMinors(5);
	}

	coordinates()->axes[X1].setLabelString("x-axis");
	coordinates()->axes[Y1].setLabelString("y-axis");
	//coordinates()->axes[Z1].setLabelString(QChar(0x38f)); // Omega - see http://www.unicode.org/charts/


	setCoordinateStyle(BOX);

	connect(&timer, SIGNAL(timeout()), this, SLOT(UpdateData()));
	timer.setInterval(100);
	timer.start();
}

void Plot::UpdateData()
{
	// change data
	for (int i = 0; i < Width1; i++)
	{
		for (int j = 1; j < Width2; j++)
		{
			data[i][j-1] = data[i][j];
		}
	}

	double xval = 50;

	for (int i = 0; i < Width1; i++)
	{
		data[i][Width2-1] = xval;

		xval += rand() % 3 - 1;

        if (rand() % 5 == 1)
            data[i][Width2-1] = std::numeric_limits<double>::quiet_NaN();
	}

	QElapsedTimer tim;
	tim.start();

	// update dataset
	createDataset(data, Width1, Width2, 0, Width1, 0, Width2);

	qint64 rate = tim.elapsed();

    setTitle(QString("Dynamic Plotter Demonstration - Frame Time %1 ms").arg(rate));

    //updateData();
	updateGL();
}

int main(int argc, char **argv)
{
	QApplication a(argc, argv);
	Plot plot;
	plot.resize(800,600);
	plot.showMaximized();
	return a.exec();
}
