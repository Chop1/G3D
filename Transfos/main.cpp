
#include <QApplication>
#include <QGLViewer/simple_viewer.h>
#include <matrices.h>
#include <primitives.h>


const Vec3 ROUGE   = {1,0.5,0};
const Vec3 VERT    = {0,1,0.5};
const Vec3 BLEU    = {0,0.8,1};
const Vec3 JAUNE   = {1,1,0};
const Vec3 CYAN    = {0,1,1};
const Vec3 MAGENTA = {1,0,1};
const Vec3 BLANC   = {1,1,1};
const Vec3 GRIS    = {0.5,0.5,0.5};
const Vec3 NOIR    = {0,0,0};

/// primitives(sphere/cube/cylindre/cone)
Primitives prim;

/// compteur (incremente dans animate)
int compteur = 0;


void fleche(const Mat4& tr, Vec3 coul)
{
    prim.draw_cylinder(tr * translate(0, 0, 1.5)*scale(0.5F, 0.5F, 2.0F), coul); // scale + transfo
    prim.draw_cone(tr * translate(0, 0, 3), coul); // pas de scale -> translation
}

void draw_repere(const Mat4& tr)
{
    prim.draw_sphere(tr * Mat4(), BLANC); // on bouge pas -> pas de transformation

    fleche(tr * rotateY(90), ROUGE);
    fleche(tr * rotateX(-90), VERT);
    fleche(tr * Mat4(), BLEU);
}

void draw_doigt(const Mat4& tr, float lg, float a) {
    prim.draw_sphere(tr * rotateZ(a) * scale(0.7f, 0.5f, 0.7f), BLANC);
    prim.draw_cube(tr * rotateZ(a) * translate(lg/2, 0, 0) * rotateY(90.0f) * scale(0.5f, 0.2f, lg), ROUGE);

    prim.draw_sphere(tr * rotateZ(a) * scale(0.7f, 0.5f, 0.7f) * translate(lg + lg/2, 0, 0), BLANC);
    prim.draw_cube(tr * rotateZ(a) * translate(lg/2 + lg, 0, 0) * rotateY(90.0f) * scale(0.5f, 0.2f, lg), VERT);

    prim.draw_sphere(tr * rotateZ(a) * scale(0.7f, 0.5f, 0.7f) * translate(2 * (lg + lg/2), 0, 0), BLANC);
    prim.draw_cube(tr * rotateZ(a) * translate(lg + lg/2 + lg, 0, 0) * rotateZ(a*2) * translate(0.01 * a * lg, 0.01 * a * lg, 0) * rotateY(90.0f) * scale(0.5f, 0.2f, lg), BLEU);
}

void draw_main(const Mat4& tr, float a) {
    prim.draw_cube(tr * scale(3, 0.4f, 3), CYAN);

    draw_doigt(translate(1.5f, 0, 1), 0.90f, a);

    draw_doigt(translate(1.5f, 0, -1), 1, a);

    draw_doigt(translate(1.5f, 0, 0), 1.5f, a);

    draw_doigt(translate(0.75f, 0, -1.5f) * rotateY(45.0f), 1, a);
}

void draw_bras(const Mat4& tr, float a) {
    draw_main(tr, a);

    prim.draw_sphere(tr * translate(-2, 0, 0), BLANC);

    prim.draw_cube(tr * translate(-4.5f, 0, 0) * scale(4, 1, 1.5f), CYAN);

    prim.draw_sphere(tr * translate(-7, 0, 0), BLANC);

    prim.draw_cube(tr * translate(-10, 0, 0) * scale(5, 2, 2), CYAN);
}

void articulations(const Mat4& transfo) {
    int d = compteur%60;
    float a = d<30?d:60-d;
    draw_bras(transfo, a);
}

int main(int argc, char *argv[])
{
	// init du viewer
	QApplication a(argc, argv);
	SimpleViewer::init_gl();
	// ouvre une fenetre QGLViewer avec fond noir et une scene de rayon 10
	SimpleViewer viewer(NOIR,10);

	// GL init
	viewer.f_init = [&] ()
	{
		prim.gl_init();
	};

	// drawing
	viewer.f_draw = [&] ()
	{
		prim.set_matrices(viewer.getCurrentModelViewMatrix(), viewer.getCurrentProjectionMatrix());

//		prim.draw_sphere(Mat4(), BLANC);
//		prim.draw_cube(translate(2,0,0), ROUGE);
//		prim.draw_cone(translate(0,2,0), VERT);
//		prim.draw_cylinder(translate(0,0,2), BLEU);

        draw_repere(Mat4());
        Mat4 rota;


        for(int i=0; i<360; i+=3)
        {
            if(i%2)
            {
                rota = rotateZ(compteur*8);
            }
            else
                rota = rotateY(compteur*10);
            for(int y=1; y<11; y+=4)
            {
                draw_repere(rotateY(compteur*2)*rotateZ(compteur*2) * rotateX(compteur*2) * (rotateY(i+compteur) * translate(0, 0, y) * scale(0.1, 0.1, 0.1) * rota));
                draw_repere(rotateY(compteur*2)*rotateZ(compteur*2) * rotateX(compteur*2) * (rotateX(i+compteur) * translate(0, 0, y+1) * scale(0.1, 0.1, 0.1) * rota));
                draw_repere(rotateY(compteur*2)*rotateZ(compteur*2) * rotateX(compteur*2) * (rotateZ(i+compteur) * translate(0, y+2, 0) * scale(0.1, 0.1, 0.1) * rota));
            }
        }

	//	articulations(Mat4());
        articulations(Mat4());
	};


	// to do when key pressed
	viewer.f_keyPress = [&] (int key, Qt::KeyboardModifiers /*mod*/)
	{
		switch(key)
		{
			case Qt::Key_A: // touche 'a'
				if (viewer.animationIsStarted())
					viewer.stopAnimation();
				else
					viewer.startAnimation();
				break;
			default:
				break;
		}
	};

	// to do every 50th of sec
	viewer.f_animate = [&] ()
	{
		compteur++;
		std::cout << "animate :" << compteur<< std::endl;
	};


	viewer.show();
	return a.exec();
}
