
#include <QApplication>
#include <QGLViewer/simple_viewer.h>
#include <matrices.h>
#include <primitives.h>
#include <meshquad.h>

const Vec3 ROUGE   = {1,0,0};
const Vec3 VERT    = {0,1,0};
const Vec3 BLEU    = {0,0,1};
const Vec3 JAUNE   = {1,1,0};
const Vec3 CYAN    = {0,1,1};
const Vec3 MAGENTA = {1,0,1};
const Vec3 BLANC   = {1,1,1};
const Vec3 GRIS    = {0.5,0.5,0.5};
const Vec3 NOIR    = {0,0,0};


void fleche(const Primitives& prim, const Mat4& tr, Vec3 coul)
{
    prim.draw_cylinder(tr * translate(0, 0, 1.5)*scale(0.5F, 0.5F, 2.0F), coul); // scale + transfo
    prim.draw_cone(tr * translate(0, 0, 3), coul); // pas de scale -> translation
}

void draw_repere(const Primitives& prim, const Mat4& tr)
{
    prim.draw_sphere(tr * Mat4() * scale(0.5), BLANC); // on bouge pas -> pas de transformation

    fleche(prim, tr * rotateY(90) * scale(0.5), ROUGE);
    fleche(prim, tr * rotateX(-90) * scale(0.5), VERT);
    fleche(prim, tr * Mat4() * scale(0.5), BLEU);
}


void star(MeshQuad& m)
{
	m.create_cube();
    for(int i = 0; i<6; i++)
    {
        for(int y = i+10; y>i; y--)
        {
            m.extrude_quad(i);
            //m.shrink_quad(i, 1.5);
            m.tourne_quad(i, 5);
        }
    }
}


int main(int argc, char *argv[])
{
	Primitives prim;
	int selected_quad = -1;
	glm::mat4 selected_frame;
	MeshQuad mesh;

	// init du viewer
	QApplication a(argc, argv);
	SimpleViewer::init_gl();
	SimpleViewer viewer({0.1,0.1,0.1},5);

	// GL init
	viewer.f_init = [&] ()
	{
		prim.gl_init();
		mesh.gl_init();
	};

	// drawing
	viewer.f_draw = [&] ()
	{
		mesh.set_matrices(viewer.getCurrentModelViewMatrix(),viewer.getCurrentProjectionMatrix());
		prim.set_matrices(viewer.getCurrentModelViewMatrix(),viewer.getCurrentProjectionMatrix());

		mesh.draw(CYAN);

		if (selected_quad>=0)
            draw_repere(prim, selected_frame);
	};

	// to do when key pressed
	viewer.f_keyPress = [&] (int key, Qt::KeyboardModifiers mod)
	{
		switch(key)
		{
			case Qt::Key_C:
                if (!(mod & Qt::ControlModifier) && !mesh.nb_quads())
                {
                    mesh.create_cube();
                }
				break;

			// e extrusion
            case Qt::Key_E:
                if(!mesh.nb_quads() || selected_quad == -1)
                {
                    std::cout <<  "pas de quad" << std::endl;
                    return;
                }
                else
                {
                    mesh.extrude_quad(selected_quad);
                }
                break;
			// +/- decale
            case Qt::Key_Plus:
                if(!mesh.nb_quads() || selected_quad == -1)
                {
                    std::cout <<  "pas de quad" << std::endl;
                    return;
                }
                else
                {
                    mesh.decale_quad(selected_quad, 1);
                }
            break;

            case Qt::Key_Minus:
                if(!mesh.nb_quads() || selected_quad == -1)
                {
                    std::cout <<  "pas de quad" << std::endl;
                    return;
                }
                else
                {
                    mesh.decale_quad(selected_quad, -1);
                }
            break;
			// z/Z shrink
            case Qt::Key_Z:
                if(!mesh.nb_quads() || selected_quad == -1)
                {
                    std::cout <<  "pas de quad" << std::endl;
                    return;
                }
                else
                {
                    mesh.shrink_quad(selected_quad, 1.5);
                }
            break;
			// t/T tourne
            case Qt::Key_T:
                if(!mesh.nb_quads() || selected_quad == -1)
                {
                    std::cout <<  "pas de quad" << std::endl;
                    return;
                }
                else
                {
                    mesh.tourne_quad(selected_quad, 25);
                }
            break;

			// Attention au cas m_selected_quad == -1

			// generation d'objet
			case Qt::Key_S:
				star(mesh);
				break;
			// ....
            case Qt::Key_M:

            break;


			default:
				break;
		}

		Vec3 sc;
		float r;
		mesh.bounding_sphere(sc,r);
		viewer.setSceneCenter(qglviewer::Vec(sc[0],sc[1],sc[2]));
		viewer.setSceneRadius(r);
		viewer.camera()->centerScene();
		viewer.update();
	};

	// to do when mouse clicked (P + Dir = demi-droite (en espace objet) orthogonale à l'écran passant par le point cliqué
	viewer.f_mousePress3D = [&] (Qt::MouseButton /*b*/, const glm::vec3& P, const glm::vec3& Dir)
	{
		selected_quad = mesh.intersected_closest(P,Dir);
		if (selected_quad>=0)
        {
			selected_frame = mesh.local_frame(selected_quad);  
            //draw_repere(selected_frame);
        }
		std::cout << selected_quad << std::endl;
	};

	viewer.clearShortcuts();
	viewer.setShortcut(QGLViewer::EXIT_VIEWER,Qt::Key_Escape);
	viewer.show();
	return a.exec();
}
