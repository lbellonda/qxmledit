/*************************************************************
 * VolumePlot extension
 *
 * (c) Sintegrial Technologies 2013-2014
 *             http://www.sintegrial.com
 ************************************************************/

#include <QApplication>
#include <QDebug>

#include <qwt3d_volumeplot.h>


using namespace Qwt3D;


class Plot : public VolumePlot
{
public:
    Plot();
};


Plot::Plot(): VolumePlot()
{
    setTitle("A Simple VolumePlot Demonstration");

    setRotation(30,0,15);
    //setScale(4,4,4);
    //setShift(0,0,15);
    //setZoom(0.6);

    Voxel::Array data;

    /*
    float r = 0, g = 0, b = 0;

    for (float x = -50; x < 50; x += 0.2)
    {
        for (float y = -30; y < 30; y += 0.3)
        {
            for (float z = -10; z < 10; z += 0.5)
            {
                data.push_back(Voxel(x, y, z, r, g, b));

                b += 0.1;
                if (b > 1) b = 0;
            }

            g += b;
            if (g > 1) g = 0;
        }

        r += 0.05;
        if (r > 1) r = 0;
    }

    */

    QFile modelFile("../data/bunny.xyz");
    if (modelFile.open(QFile::ReadOnly))
    {
        qDebug() << "opened: " << modelFile.fileName();

        float x,y,z;
        float d,a;

        QTextStream ts(&modelFile);
        while (!ts.atEnd())
        {
            ts >> x; ts >> z; ts >> y;
            ts >> d; ts >> a;
            data.push_back(Voxel(x, y, z, d*255, (d+a)*255, a*255));
        }
    }
    else
        qDebug() << "failed: " << modelFile.fileName();


    qDebug() << "points: " << data.size();

    createDataset(data);

    for (unsigned i=0; i!=coordinates()->axes.size(); ++i)
    {
        coordinates()->axes[i].setMajors(10);
        coordinates()->axes[i].setMinors(5);
    }

    coordinates()->axes[X1].setLabelString("x-axis");
    coordinates()->axes[Y1].setLabelString("y-axis");
    coordinates()->axes[Z1].setLabelString("z-axis");


    setCoordinateStyle(FRAME);

    updateData();
    updateGL();
}

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    Plot plot;
    plot.resize(800,600);
    plot.show();
    return a.exec();
}
