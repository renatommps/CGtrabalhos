/* 
 * File:   main.cpp
 * Author: renato
 *
 * Created on 12 de Agosto de 2016, 19:04
 */

#include <iostream>     //std::cout
#include <vector>       //std::vector
#include <cstdlib>      //std::atoi, std::atof
#include <string.h>     //std::strdup
#include <gtk/gtk.h>
#include "DisplayFile.h"
#include "GeometricObject.h"
#include "Window.h"
#include "Polygon.h"
#include "Line.h"
#include "Point.h"
#include "Clipping.h"

/* +++++++++++++++++++++++++++++++ CONSTANTS ++++++++++++++++++++++++++++++++ */
static const double MAIN_WINDOW_WIDTH = 800;
static const double MAIN_WINDOW_HEIGHT = 500;

static const double VIEW_PORT_WIDTH = 500;
static const double VIEW_PORT_HEIGHT = 400;

static const double OBJECT_VIEWER_WIDTH = 100;
static const double OBJECT_VIEWER_HEIGHT = 100;
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* +++++++++++++++++++++++++++ GLOBAL VARIABLES +++++++++++++++++++++++++++++ */
/* Surface to store current scribbles */
static cairo_surface_t *surface = NULL;
DisplayFile displayFile;
Window window(0.0, 0.0, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT, &displayFile);
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* ++++++++++++++++++++++++ STATIC METHODS DECLARATION ++++++++++++++++++++++ */
static void close_window(void);
static void clear_surface(void);
static void defineDrawingParameters(cairo_t *cr,
        double backgrdR, double backgrdG, double backgrdB,
        double LineR, double LineG, double LineB, double lineWidth);
static void actionAddObject(GtkButton* button, gpointer data);
static void actionAddPointsToObject(GtkButton* button, GtkWidget* pWindow);
static void actionTranslateObject(GtkWidget *widget, gpointer user_data);
static void actionScaleObject(GtkWidget *widget, gpointer user_data);
static void actionRotateObjectToPointReference(GtkWidget *widget, gpointer user_data);
static void actionRotateObjectToOriginReference(GtkWidget *widget, gpointer user_data);
static void actionRotateObjectToHimSelfReference(GtkWidget *widget, gpointer user_data);
static void actionMoveStep(GtkWidget *widget, gpointer user_data);
static void actionMoveUp(GtkWidget *widget, gpointer user_data);
static void actionMoveIn(GtkWidget *widget, gpointer user_data);
static void actionMoveLeft(GtkWidget *widget, gpointer user_data);
static void actionMoveRight(GtkWidget *widget, gpointer user_data);
static void actionMoveDown(GtkWidget *widget, gpointer user_data);
static void actionMoveOut(GtkWidget *widget, gpointer user_data);
static void actionRotate(GtkWidget *widget, gpointer user_data);
static GtkTreeModel * createAndFillModel(void);
static GtkWidget * createViewAndModel(void);
static void addObjectToListViewer(std::string objName, int objNumPoints, GtkTreeView* treeview);
static gboolean drawDisplayFiles(GtkWidget *widget, cairo_t *cr, gpointer data);
static void treeSelectionChanged(GtkTreeSelection *selection, gpointer data);
double ViewPortTransformationX(double xw);
double ViewPortTransformationY(double yw);
std::vector<GeometricObject> cliping(std::vector<GeometricObject>);

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

enum {
    COL_NAME = 0,
    COL_NUM_POINTS,
    NUM_COLS
};

typedef struct createObjectArgs {
    GtkWidget* name;
    GtkWidget* numberOfPoints;
    GtkTreeView *view;
    std::vector<GtkWidget *> *pointXEntryVector;
    std::vector<GtkWidget *> *pointYEntryVector;

    // This is C++ in all of its glory.

    createObjectArgs(GtkWidget* entryName, GtkWidget* entryNumPoints, GtkTreeView* treeViwer) :
    name(entryName), numberOfPoints(entryNumPoints), view(treeViwer) {
        pointXEntryVector = new std::vector<GtkWidget*>();
        pointYEntryVector = new std::vector<GtkWidget*>();
    }

    // This is the destructor.  Will delete the array of vertices, if present.

    ~createObjectArgs() {
        if (name) delete[] name;
        if (numberOfPoints) delete[] numberOfPoints;
        if (pointXEntryVector) delete[] pointXEntryVector;
        if (pointYEntryVector) delete[] pointYEntryVector;
    }
} createObjectArgs;

typedef struct ObjectOperationsParametres {
    GtkWidget* selectedObject;
    GtkWidget* translateValueX;
    GtkWidget* translateValueY;
    GtkWidget* scaleValueX;
    GtkWidget* scaleValueY;
    GtkWidget* rotationAngle;
    GtkWidget* rotateToPointValueX;
    GtkWidget* rotateToPointValueY;


    // This is C++ in all of its glory.

    ObjectOperationsParametres(GtkWidget* entrySelectedObject, GtkWidget* entryTranslateValueX,
            GtkWidget* entryTranslateValueY, GtkWidget* entryScaleValueX, GtkWidget* entryScaleValueY,
            GtkWidget* entryRotationAngle, GtkWidget* entryRotateToPointValueX, GtkWidget* entryRotateToPointValueY) :

    selectedObject(entrySelectedObject),
    translateValueX(entryTranslateValueX),
    translateValueY(entryTranslateValueY),
    scaleValueX(entryScaleValueX),
    scaleValueY(entryScaleValueY),
    rotationAngle(entryRotationAngle),
    rotateToPointValueX(entryRotateToPointValueX),
    rotateToPointValueY(entryRotateToPointValueY) {
    }

    // This is the destructor.

    ~ObjectOperationsParametres() {
    }
} ObjectOperationsParametres;

int main(int argc, char **argv) {

    /* ++++++++++++++++++++ OBJETOS INSERIDOS PARA TESTES E VISUALIZAÇÃO +++++++++++++++++++++++ */
    /* define uma linha e um polígono para visualização rápida para testes */
    Line line1("line1", 0.0, 0.0, MAIN_WINDOW_WIDTH / 2, MAIN_WINDOW_HEIGHT / 2);
    Polygon polygon1("Polygon1",{Point(0.0, 0.0), Point(0.0, MAIN_WINDOW_HEIGHT / 2),
        Point(MAIN_WINDOW_WIDTH / 2, MAIN_WINDOW_HEIGHT / 2), Point(MAIN_WINDOW_WIDTH / 2, 0.0)});

    /* adiciona os objetos criados no DiplayFile */
    displayFile.addObject(line1);
    displayFile.addObject(polygon1);
    /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


    /* ++++++++++ DECLARAÇÃO DOS COMPONENTES GRÁFICOS E LÓGICOS ++++++++++++++ */
    GtkWidget *mainWindow;
    GtkWidget *grid;

    GtkWidget *buttonAddObject;

    GtkWidget *labelObjectsList;
    GtkWidget *treeViewerObjectsList;
    GtkTreeSelection *treeViewerSelected;

    GtkWidget *labelObjectsOperations;
    GtkWidget *labelSelectedObject;
    GtkWidget *entrySelectedObject;
    GtkWidget *buttonTranslateObject;
    GtkWidget *labelTranslateValueX;
    GtkWidget *entryTranslateValueX;
    GtkWidget *labelTranslateValueY;
    GtkWidget *entryTranslateValueY;
    GtkWidget *buttonScaleObject;
    GtkWidget *labelScaleValueX;
    GtkWidget *entryScaleValueX;
    GtkWidget *labelScaleValueY;
    GtkWidget *entryScaleValueY;
    GtkWidget *labelRotationAngle;
    GtkWidget *entryRotationAngle;
    GtkWidget *buttonRotateObjectRelateToPoint;
    GtkWidget *labelRotateToPointValueX;
    GtkWidget *entryRotateToPointValueX;
    GtkWidget *labelRotateToPointValueY;
    GtkWidget *entryRotateToPointValueY;
    GtkWidget *buttonRotateObjectRelateToOrigen;
    GtkWidget *buttonRotateObjectRelateToObject;

    GtkWidget *labelWindowOperations;
    GtkWidget *labelWindowMovementStep;
    GtkWidget *entryWindowStep;
    GtkWidget *buttonWindowGoUp;
    GtkWidget *buttonWindowGoLeft;
    GtkWidget *buttonWindowGoRight;
    GtkWidget *buttonWindowGoDown;
    GtkWidget *labelWindowZoomQuantity;
    GtkWidget *entryWindowZoomQuantity;
    GtkWidget *buttonWindowZoomIn;
    GtkWidget *buttonWindowZoomOut;
    GtkWidget *buttonWindowRotate;
    GtkWidget *labelWindowRotate;
    GtkWidget *entryWindowRotationAngle;

    GtkWidget *viewPort;
    GtkWidget *entryLogViewr;
    /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


    /* ++++++++++++++++++++++++ INICIALIZAÇÃO DO GTK +++++++++++++++++++++++++ */
    gtk_init(&argc, &argv);
    /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


    /* +++++++++++ DEFINIÇÃO DOS COMPONENTES GRÁFICOS E LÓGICOS ++++++++++++++ */
    mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    grid = gtk_grid_new();

    buttonAddObject = gtk_button_new_with_label("Adiciona Objeto");

    labelObjectsList = gtk_label_new("Objetos gráficosadicionados");
    treeViewerObjectsList = createViewAndModel();
    treeViewerSelected = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeViewerObjectsList));

    labelObjectsOperations = gtk_label_new("Operações sobreobjetos gráficos");
    labelSelectedObject = gtk_label_new("Objeto selecionado:");
    entrySelectedObject = gtk_entry_new();
    buttonTranslateObject = gtk_button_new_with_label("Transladar");
    //labelTranslateValueX = gtk_label_new("quant. X:");
    entryTranslateValueX = gtk_entry_new();
    //labelTranslateValueY = gtk_label_new("quant. Y:");
    entryTranslateValueY = gtk_entry_new();
    buttonScaleObject = gtk_button_new_with_label("Escalonar");

    //labelScaleValueX = gtk_label_new("quant. X:");
    entryScaleValueX = gtk_entry_new();
    //labelScaleValueY = gtk_label_new("quant. Y:");
    entryScaleValueY = gtk_entry_new();
    labelRotationAngle = gtk_label_new("ângulo:");
    entryRotationAngle = gtk_entry_new();
    buttonRotateObjectRelateToPoint = gtk_button_new_with_label("Rotacionar em relação\nao ponto informado");
    labelRotateToPointValueX = gtk_label_new("cord. X:");
    entryRotateToPointValueX = gtk_entry_new();
    labelRotateToPointValueY = gtk_label_new("cord. Y:");
    entryRotateToPointValueY = gtk_entry_new();
    buttonRotateObjectRelateToOrigen = gtk_button_new_with_label("Rotacionar em relação à origem");
    buttonRotateObjectRelateToObject = gtk_button_new_with_label("Rotacionar em relação ao\ncentro do objeto selecionado");

    labelWindowOperations = gtk_label_new("Operações sobre a janela de visualização");
    labelWindowMovementStep = gtk_label_new("Passo:");
    entryWindowStep = gtk_entry_new();
    buttonWindowGoUp = gtk_button_new_with_label("Up");
    buttonWindowGoLeft = gtk_button_new_with_label("Left");
    buttonWindowGoRight = gtk_button_new_with_label("Right");
    buttonWindowGoDown = gtk_button_new_with_label("Down");
    labelWindowZoomQuantity = gtk_label_new("Zoom (%):");
    entryWindowZoomQuantity = gtk_entry_new();
    buttonWindowZoomIn = gtk_button_new_with_label("In");
    buttonWindowZoomOut = gtk_button_new_with_label("Out");
    labelWindowRotate = gtk_label_new("Ângulo:");
    entryWindowRotationAngle = gtk_entry_new();
    buttonWindowRotate = gtk_button_new_with_label("Rotacionar");

    viewPort = gtk_drawing_area_new();
    entryLogViewr = gtk_entry_new();
    /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


    /* +++ DEFINIÇÃO DO TÍTULO, TAMANHA, BORDA E SINAL DA JANELA PRINCIPAL +++ */
    gtk_window_set_title(GTK_WINDOW(mainWindow), "Fancy Objects Viewer");
    gtk_window_set_default_size(GTK_WINDOW(mainWindow), MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
    g_signal_connect(mainWindow, "destroy", G_CALLBACK(close_window), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(mainWindow), 5);
    gtk_window_set_position(GTK_WINDOW(mainWindow), GTK_WIN_POS_CENTER);
    /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


    /* ++++ DEFINIÇÃO DO COMPONENTE QUE REFERENCIA OBJETOS SELECIONADOS NA LISTA DE OBJETOS +++ */
    gtk_tree_selection_set_mode(treeViewerSelected, GTK_SELECTION_SINGLE);
    g_signal_connect(G_OBJECT(treeViewerSelected), "changed", G_CALLBACK(treeSelectionChanged), (gpointer) entrySelectedObject);
    /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


    /* ++++++++++++++++++ DEFINIÇÃO DOS COMPONENTES DE INPUT ++++++++++++++++++ */
    gtk_entry_set_max_length(GTK_ENTRY(entrySelectedObject), 20);
    gtk_entry_set_max_length(GTK_ENTRY(entryTranslateValueX), 10);
    gtk_entry_set_max_length(GTK_ENTRY(entryTranslateValueY), 10);
    gtk_entry_set_max_length(GTK_ENTRY(entryScaleValueX), 10);
    gtk_entry_set_max_length(GTK_ENTRY(entryScaleValueY), 10);
    gtk_entry_set_max_length(GTK_ENTRY(entryRotationAngle), 10);
    gtk_entry_set_max_length(GTK_ENTRY(entryRotateToPointValueX), 10);
    gtk_entry_set_max_length(GTK_ENTRY(entryRotateToPointValueY), 10);
    gtk_entry_set_max_length(GTK_ENTRY(entryWindowStep), 10);
    gtk_entry_set_max_length(GTK_ENTRY(entryWindowZoomQuantity), 10);
    gtk_entry_set_max_length(GTK_ENTRY(entryWindowRotationAngle), 10);

    gtk_editable_set_editable(GTK_EDITABLE(entrySelectedObject), FALSE);
    gtk_editable_set_editable(GTK_EDITABLE(entryLogViewr), FALSE);
    /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


    /* ++++++++++ DEFINIÇÃO DO TAMANHO E DO SINAL DA VIEW PORT ++++++++++++++++ */
    gtk_widget_set_size_request(viewPort, VIEW_PORT_WIDTH, VIEW_PORT_HEIGHT);
    g_signal_connect(viewPort, "draw", G_CALLBACK(drawDisplayFiles), NULL);
    /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


    /* +++ INSTANCIAÇÃO DA STRUCT QUE CONTÉM PARAMETROS PARA AS FUNÇÕES SOBRE OBJETOS GRÁFICOS ++++ */
    ObjectOperationsParametres* objParameters = new ObjectOperationsParametres(
            entrySelectedObject,
            entryTranslateValueX,
            entryTranslateValueY,
            entryScaleValueX,
            entryScaleValueY,
            entryRotationAngle,
            entryRotateToPointValueX,
            entryRotateToPointValueY
            );
    /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


    /* ++++++++++++++++++++++++++++++ DEFINIÇÃO DOS SINAIS DOS BOTÕES ++++++++++++++++++++++++++++++++++++++++++ */
    g_signal_connect(buttonAddObject, "clicked", G_CALLBACK(actionAddObject), (gpointer) treeViewerObjectsList);

    g_signal_connect(buttonTranslateObject, "clicked", G_CALLBACK(actionTranslateObject), (gpointer) objParameters);
    g_signal_connect(buttonScaleObject, "clicked", G_CALLBACK(actionScaleObject), (gpointer) objParameters);
    g_signal_connect(buttonRotateObjectRelateToPoint, "clicked", G_CALLBACK(actionRotateObjectToPointReference), (gpointer) objParameters);
    g_signal_connect(buttonRotateObjectRelateToOrigen, "clicked", G_CALLBACK(actionRotateObjectToOriginReference), (gpointer) objParameters);
    g_signal_connect(buttonRotateObjectRelateToObject, "clicked", G_CALLBACK(actionRotateObjectToHimSelfReference), (gpointer) objParameters);

    g_signal_connect(buttonWindowGoUp, "clicked", G_CALLBACK(actionMoveUp), (gpointer) entryWindowStep);
    g_signal_connect(buttonWindowGoLeft, "clicked", G_CALLBACK(actionMoveLeft), (gpointer) entryWindowStep);
    g_signal_connect(buttonWindowGoRight, "clicked", G_CALLBACK(actionMoveRight), (gpointer) entryWindowStep);
    g_signal_connect(buttonWindowGoDown, "clicked", G_CALLBACK(actionMoveDown), (gpointer) entryWindowStep);
    g_signal_connect(buttonWindowZoomIn, "clicked", G_CALLBACK(actionMoveIn), (gpointer) entryWindowStep);
    g_signal_connect(buttonWindowZoomOut, "clicked", G_CALLBACK(actionMoveOut), (gpointer) entryWindowStep);
    g_signal_connect(buttonWindowRotate, "clicked", G_CALLBACK(actionRotate), (gpointer) entryWindowRotationAngle);
    /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


    /* +++++++++++++++++ ADICIONA O GRID À JANELA PRINCIPAL ++++++++++++++++++ */
    gtk_container_add(GTK_CONTAINER(mainWindow), grid);
    /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


    /* ++++++++++++++++ ADICIONA OS OBJETOS GRÁFICOS AO GRID +++++++++++++++++ */
    /* cada objeto adicionado possúi 4 parâmetros numéricos para serem definidos,
     são eles: 1° - coluna, 2° linha, 3° - lagura (número de colunas que ocupa),
     * 4° - altura (número de linhas que ocupa) */

    int row = 0; // variável auxiliar para adicionar os objetos em sequência

    gtk_grid_attach(GTK_GRID(grid), buttonAddObject, 0, row++, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), labelObjectsList, 0, row++, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), treeViewerObjectsList, 0, row++, 3, 1);

    gtk_grid_attach(GTK_GRID(grid), labelObjectsOperations, 0, row++, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), labelSelectedObject, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entrySelectedObject, 1, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), buttonTranslateObject, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entryTranslateValueX, 1, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entryTranslateValueY, 2, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), buttonScaleObject, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entryScaleValueX, 1, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entryScaleValueY, 2, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), labelRotationAngle, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entryRotationAngle, 1, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), buttonRotateObjectRelateToPoint, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entryRotateToPointValueX, 1, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entryRotateToPointValueY, 2, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), buttonRotateObjectRelateToOrigen, 0, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), buttonRotateObjectRelateToObject, 0, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), labelWindowOperations, 0, row++, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), labelWindowMovementStep, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entryWindowStep, 1, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), buttonWindowGoUp, 1, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), buttonWindowGoLeft, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), buttonWindowGoRight, 2, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), buttonWindowGoDown, 1, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), labelWindowZoomQuantity, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entryWindowZoomQuantity, 1, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), buttonWindowZoomIn, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), buttonWindowZoomOut, 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), buttonWindowRotate, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), labelWindowRotate, 1, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entryWindowRotationAngle, 2, row++, 1, 1);

    int viewPortWidth = 12;
    int viewPortHeight = row - 2;
    gtk_grid_attach(GTK_GRID(grid), viewPort, 4, 0, viewPortWidth, viewPortHeight);
    gtk_grid_attach(GTK_GRID(grid), entryLogViewr, 4, --row, viewPortWidth, 1);
    /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

    /* MOSTRA TODOS OS COMPONENTES CRIADOS NA MAIN WINDOW */
    gtk_widget_show_all(mainWindow);

    /* CHAMA A FUNÇÃO PRINICPAL QUE INICIA O LOOP PRINCIPAL DO GTK */
    gtk_main();

    return 0;
}

static void close_window(void) {
    if (surface)
        cairo_surface_destroy(surface);

    gtk_main_quit();
}

static void clear_surface(void) {
    cairo_t *cr;

    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    cairo_destroy(cr);
}

static void defineDrawingParameters(cairo_t *cr,
        double backgrdR, double backgrdG, double backgrdB,
        double LineR, double LineG, double LineB, double lineWidth) {

    /* Set color for background */
    cairo_set_source_rgb(cr, backgrdR, backgrdG, backgrdB);
    /* fill in the background color*/
    cairo_paint(cr);
    /* Set line color */
    cairo_set_source_rgb(cr, LineR, LineG, LineB);
    /* Set line widht */
    cairo_set_line_width(cr, lineWidth);
}

static void createObjectWindowDestroy(GtkWidget* widget, gpointer data) {
    createObjectArgs* args = (createObjectArgs*) data;
    free(args);
}

static void confirmObjectInsertion(GtkWidget *widget, gpointer data) {
    createObjectArgs* objArgs = (createObjectArgs*) data;

    std::string objName = gtk_entry_get_text(GTK_ENTRY(objArgs->name));
    int objNumPoints = std::atoi(gtk_entry_get_text(GTK_ENTRY(objArgs->numberOfPoints)));

    std::vector<GtkWidget *> *pointXVector = objArgs->pointXEntryVector;
    std::vector<GtkWidget *> *pointYVector = objArgs->pointYEntryVector;
    std::vector<Point> points;

    std::cout << "nome: " << objName << std::endl;
    std::cout << "número de pontos: " << objNumPoints << std::endl;

    for (int i = 0; i < objNumPoints; i++) {
        GtkWidget * widgetPontoX = pointXVector->at(i);
        GtkWidget * widgetPontoY = pointYVector->at(i);

        double px = std::atof(gtk_entry_get_text(GTK_ENTRY(widgetPontoX)));
        double py = std::atof(gtk_entry_get_text(GTK_ENTRY(widgetPontoY)));

        Point p(px, py);
        points.push_back(p);
    }

    GeometricObject myObj(objName, points);

    displayFile.addObject(myObj);

    /* adiciona o objeto criado na tree viewer */
    addObjectToListViewer(objName, objNumPoints, objArgs->view);
}

static void readObjectPoints(GtkWidget *widget, gpointer data) {
    createObjectArgs* objArgs = (createObjectArgs*) data;

    GtkWidget * window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget * grid = gtk_grid_new();
    GtkWidget * confirmButton = gtk_button_new_with_label("Confirma");
    GtkWidget * cancelButton = gtk_button_new_with_label("Cancela");

    gtk_window_set_title(GTK_WINDOW(window), "Adiciona Pontos");
    gtk_window_set_default_size(GTK_WINDOW(window), 120, 50);

    gtk_container_add(GTK_CONTAINER(window), grid);

    int objNumPoints = std::atoi(gtk_entry_get_text(GTK_ENTRY(objArgs->numberOfPoints)));

    std::vector<GtkWidget *> *pointXEntryVector = new std::vector<GtkWidget *>();
    std::vector<GtkWidget *> *pointYEntryVector = new std::vector<GtkWidget *>();
    std::vector<GtkWidget *> pointLabelVector;

    int gridCoord1 = 0;
    int gridCoord2 = 0;

    for (int i = 0; i < objNumPoints; i++) {
        std::string labelName("Entre cordenadas X e Y do ponto " + std::to_string(i));
        GtkWidget* label = gtk_label_new(labelName.c_str());
        GtkWidget* coordX = gtk_entry_new();
        GtkWidget* coordY = gtk_entry_new();

        pointXEntryVector->push_back(coordX);
        pointYEntryVector->push_back(coordY);
        pointLabelVector.push_back(label);

        gtk_grid_attach(GTK_GRID(grid), label, gridCoord1++, gridCoord2, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), coordX, gridCoord1++, gridCoord2, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), coordY, gridCoord1, gridCoord2++, 1, 1);
        gridCoord1 = 0;
    }

    gtk_grid_attach(GTK_GRID(grid), cancelButton, ++gridCoord1, gridCoord2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), confirmButton, ++gridCoord1, gridCoord2, 1, 1);

    objArgs->pointXEntryVector = pointXEntryVector;
    objArgs->pointYEntryVector = pointYEntryVector;

    gtk_widget_show_all(window);

    g_signal_connect(confirmButton, "clicked", G_CALLBACK(confirmObjectInsertion), (gpointer) objArgs);
    g_signal_connect_swapped(cancelButton, "clicked", G_CALLBACK(gtk_widget_destroy), window);
}

static void actionAddObject(GtkButton* button, gpointer data) {
    g_print("O botao \"Adiciona objeto\" foi clicado\n");

    GtkTreeView *treeview = (GtkTreeView*) data;

    GtkWidget* window;
    GtkWidget* confirmButton;
    GtkWidget* cancelButton;
    GtkWidget* grid;
    GtkWidget* labelObjName;
    GtkWidget* labelObjNumPoints;
    GtkWidget* entryObjName;
    GtkWidget* entryObjNumPoints;

    labelObjName = gtk_label_new("Nome do objeto:");
    labelObjNumPoints = gtk_label_new("Número de pontos do objeto:");

    entryObjName = gtk_entry_new();
    entryObjNumPoints = gtk_entry_new();

    gtk_entry_set_text(GTK_ENTRY(entryObjName), "Objeto nome");
    gtk_entry_set_text(GTK_ENTRY(entryObjNumPoints), "0");

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Criar polígono");
    gtk_window_set_default_size(GTK_WINDOW(window), 120, 50);

    grid = gtk_grid_new();
    confirmButton = gtk_button_new_with_label("Confirma");
    cancelButton = gtk_button_new_with_label("Cancela");

    gtk_container_add(GTK_CONTAINER(window), grid);
    gtk_grid_attach(GTK_GRID(grid), labelObjName, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entryObjName, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), labelObjNumPoints, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entryObjNumPoints, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cancelButton, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), confirmButton, 1, 2, 1, 1);

    createObjectArgs* objEntryArgs = new createObjectArgs(entryObjName, entryObjNumPoints, treeview);

    gtk_widget_show_all(window);

    g_signal_connect(confirmButton, "clicked", G_CALLBACK(readObjectPoints), (gpointer) objEntryArgs);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), (gpointer) window);
    g_signal_connect_swapped(cancelButton, "clicked", G_CALLBACK(gtk_widget_destroy), window);
}

static void actionTranslateObject(GtkWidget *widget, gpointer user_data) {
    ObjectOperationsParametres* args = (ObjectOperationsParametres*) user_data;

    GtkWidget* valueX = args->translateValueX;
    GtkWidget* valueY = args->translateValueY;
    GtkWidget* obj = args->selectedObject;

    double dx = std::atof(gtk_entry_get_text(GTK_ENTRY(valueX)));
    double dy = std::atof(gtk_entry_get_text(GTK_ENTRY(valueY)));
    std::string objName = gtk_entry_get_text(GTK_ENTRY(obj));

    displayFile.translateObject(dx, dy, objName);

    g_print("O botao \"Tranladar\" foi clicado, nome do objeto: %s, valor x: %f, valor y: %f\n", objName.c_str(), dx, dy);
}

static void actionScaleObject(GtkWidget *widget, gpointer user_data) {
    ObjectOperationsParametres* args = (ObjectOperationsParametres*) user_data;

    GtkWidget* valueX = args->scaleValueX;
    GtkWidget* valueY = args->scaleValueY;
    GtkWidget* obj = args->selectedObject;

    double sx = std::atof(gtk_entry_get_text(GTK_ENTRY(valueX)));
    double sy = std::atof(gtk_entry_get_text(GTK_ENTRY(valueY)));
    std::string objName = gtk_entry_get_text(GTK_ENTRY(obj));

    displayFile.scaleObject(sx, sy, objName);

    g_print("O botao \"Escalonar\" foi clicado, nome do objeto: %s, valor x: %f, valor y: %f\n", objName.c_str(), sx, sy);
}

static void actionRotateObjectToPointReference(GtkWidget *widget, gpointer user_data) {
    ObjectOperationsParametres* args = (ObjectOperationsParametres*) user_data;

    GtkWidget* valueX = args->rotateToPointValueX;
    GtkWidget* valueY = args->rotateToPointValueY;
    GtkWidget* entryAngle = args->rotationAngle;
    GtkWidget* obj = args->selectedObject;

    double coordX = std::atof(gtk_entry_get_text(GTK_ENTRY(valueX)));
    double coordY = std::atof(gtk_entry_get_text(GTK_ENTRY(valueY)));
    double angle = std::atof(gtk_entry_get_text(GTK_ENTRY(entryAngle)));
    std::string objName = gtk_entry_get_text(GTK_ENTRY(obj));

    displayFile.rotateObjectToPointReference(coordX, coordY, angle, objName);

    g_print("O botao \"Rotacionar em relação ao ponto informado\" foi clicado, nome do objeto: %s, x: %f, y: %f, angulo: %f \n",
            objName.c_str(), coordX, coordY, angle);
}

static void actionRotateObjectToOriginReference(GtkWidget *widget, gpointer user_data) {
    ObjectOperationsParametres* args = (ObjectOperationsParametres*) user_data;

    GtkWidget* entryAngle = args->rotationAngle;
    GtkWidget* obj = args->selectedObject;

    double coordX = 0;
    double coordY = 0;
    double angle = std::atof(gtk_entry_get_text(GTK_ENTRY(entryAngle)));
    std::string objName = gtk_entry_get_text(GTK_ENTRY(obj));

    displayFile.rotateObjectToPointReference(coordX, coordY, angle, objName);

    g_print("O botao \"Rotacionar em relação à origem\" foi clicado, nome do objeto: %s, angulo: %f \n",
            objName.c_str(), angle);
}

static void actionRotateObjectToHimSelfReference(GtkWidget *widget, gpointer user_data) {
    ObjectOperationsParametres* args = (ObjectOperationsParametres*) user_data;

    GtkWidget* entryAngle = args->rotationAngle;
    GtkWidget* obj = args->selectedObject;

    double angle = std::atof(gtk_entry_get_text(GTK_ENTRY(entryAngle)));
    std::string objName = gtk_entry_get_text(GTK_ENTRY(obj));

    displayFile.rotateObject(angle, objName);

    g_print("O botao \"Rotacionar em relação ao centro do objeto selecionado\" foi clicado, nome do objeto: %s, angulo: %f \n",
            objName.c_str(), angle);
}

static void actionMoveStep(GtkWidget *widget, gpointer user_data) {
    GtkEntry* e = (GtkEntry*) user_data;
    const gchar* entry = gtk_entry_get_text(e);
    double r = atof(entry);
    g_print("O botao \"Passo\" foi clicado\n");
    g_print("O valor do passo é de %.2f\n", r);
}

static void actionMoveUp(GtkWidget *widget, gpointer user_data) {
    GtkEntry* e = (GtkEntry*) user_data;
    const gchar* entry = gtk_entry_get_text(e);
    double value = atof(entry);

    g_print("O botao \"Up\" foi clicado\n");
    window.moveUp(value);
}

static void actionMoveDown(GtkWidget *widget, gpointer user_data) {
    GtkEntry* e = (GtkEntry*) user_data;
    const gchar* entry = gtk_entry_get_text(e);
    double value = atof(entry);

    g_print("O botao \"Down\" foi clicado\n");
    window.moveDown(value);
}

static void actionMoveLeft(GtkWidget *widget, gpointer user_data) {
    GtkEntry* e = (GtkEntry*) user_data;
    const gchar* entry = gtk_entry_get_text(e);
    double value = atof(entry);

    g_print("O botao \"Left\" foi clicado\n");
    window.moveLeft(value);
}

static void actionMoveRight(GtkWidget *widget, gpointer user_data) {
    GtkEntry* e = (GtkEntry*) user_data;
    const gchar* entry = gtk_entry_get_text(e);
    double value = atof(entry);

    g_print("O botao \"Right\" foi clicado\n");
    window.moveRight(value);
}

static void actionMoveIn(GtkWidget *widget, gpointer user_data) {
    GtkEntry* e = (GtkEntry*) user_data;
    const gchar* entry = gtk_entry_get_text(e);
    double value = atof(entry);

    g_print("O botao \"In\" foi clicado\n");
    window.zoomIn(value);
}

static void actionMoveOut(GtkWidget *widget, gpointer user_data) {
    GtkEntry* e = (GtkEntry*) user_data;
    const gchar* entry = gtk_entry_get_text(e);
    double value = atof(entry);

    g_print("O botao \"Out\" foi clicado\n");
    window.zoomOut(value);
}

static void actionRotate(GtkWidget *widget, gpointer user_data) {
    GtkEntry* e = (GtkEntry*) user_data;
    const gchar* entry = gtk_entry_get_text(e);
    double value = atof(entry);

    g_print("O botao \"Rotacionar\" foi clicado\n");
    window.rotate(value);
}

static void treeSelectionChanged(GtkTreeSelection *selection, gpointer data) {
    GtkEntry* selectedObj = (GtkEntry*) data;
    GtkTreeIter iter;
    GtkTreeModel *model;
    gchar *name;
    gint numPoints;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_tree_model_get(model, &iter, COL_NAME, &name, COL_NUM_POINTS, &numPoints, -1);
        gtk_entry_set_text(GTK_ENTRY(selectedObj), name);
        g_print("You selected an object called %s with %d points\n", name, numPoints);

        g_free(name);
    }
}

static GtkTreeModel * createAndFillModel(void) {
    GtkListStore *store;
    GtkTreeIter iter;

    store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_UINT);

    /* vetor de objetos a serem listados*/
    std::vector<GeometricObject> graficObjects = (*displayFile.getObjects());

    g_print("%d objects will be added to the tree viewer\n", (int) graficObjects.size());

    /* itera sobre todos os objetos que serão listados*/
    for (GeometricObject obj : graficObjects) {

        /* pega o nome do objeto*/
        std::string objName = obj.getName();
        /* pega o número de pontos do objeto*/
        int objNumPoints = obj.getNumPoints();

        /* Append a row and fill with the name and number of points data */
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                COL_NAME, objName.c_str(),
                COL_NUM_POINTS, objNumPoints,
                -1);
    }

    return GTK_TREE_MODEL(store);
}

static GtkWidget * createViewAndModel(void) {
    GtkCellRenderer *renderer;
    GtkTreeModel *model;
    GtkWidget *view;

    view = gtk_tree_view_new();

    /* --- Column #1 --- */

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
            -1,
            "Nome",
            renderer,
            "text", COL_NAME,
            NULL);

    /* --- Column #2 --- */

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
            -1,
            "Número de pontos",
            renderer,
            "text", COL_NUM_POINTS,
            NULL);

    model = createAndFillModel();

    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);

    /* The tree view has acquired its own reference to the
     *  model, so we can drop ours. That way the model will
     *  be freed automatically when the tree view is destroyed */

    g_object_unref(model);

    return view;
}

static void addObjectToListViewer(std::string objName, int objNumPoints, GtkTreeView* treeview) {
    GtkTreeIter iter;
    GtkTreeModel *model;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));

    /* Append a row and fill with the name and number of points data */
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_NAME, objName.c_str(),
            COL_NUM_POINTS, objNumPoints,
            -1); //indicates the end

    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), model);

    g_print("List viewer refreshed!\n");
}

static gboolean drawDisplayFiles(GtkWidget *widget, cairo_t *cr, gpointer data) {

    /* vetor de objetos a serem desenhados*/
    std::vector<GeometricObject> graficObjects = (*displayFile.getObjects());

    defineDrawingParameters(cr, 1, 1, 1, 0, 0, 0, 1);

    //std::vector<GeometricObject> clipedObjects = Clipping::clip(graficObjects, &window);
    Clipping clipper(graficObjects, &window);
    std::vector<GeometricObject> clipedObjects = clipper.clip();


    /* itera sobre todos os objetos que serão desenhados*/
    for (GeometricObject obj : clipedObjects) {

        /* pega todos os pontos do objeto*/
        std::vector<Point> objPoints = obj.getPointsVector();
        /* pega o número de pontos do objeto*/
        int objNumPoints = objPoints.size();

        /* se tiver menos de dois pontos, quer dizer que não é possível desenhar
         nem uma reta, no caso, o objeto seria apenas um ponto */
        if (objNumPoints < 2) {
            break;
        }

        Point p1, p2;
        double xp1, yp1;
        double xp2, yp2;

        /* define o ponto inicial (primeiro ponto do objeto) */
        p1 = objPoints[0];
        xp1 = ViewPortTransformationX(p1.getX());
        yp1 = ViewPortTransformationY(p1.getY());

        /* move para o ponto inicial */
        cairo_move_to(cr, xp1, yp1);

        /* itera sobre todos os pontos do objeto */
        for (int i = 1; i < objNumPoints; i++) {
            p2 = objPoints[i];
            xp2 = ViewPortTransformationX(p2.getX());
            yp2 = ViewPortTransformationY(p2.getY());
            cairo_line_to(cr, xp2, yp2);
        }
        /* fecha o caminho (liga o último ponto ao primeiro) */
        cairo_close_path(cr);
        /* desenha efetivamente no caminho formado*/
        cairo_stroke(cr);

        gtk_widget_queue_draw(widget);
    }
    return FALSE;
}

//std::vector<GeometricObject> cliping(std::vector<GeometricObject>) {
//
//    std::vector<GeometricObject> clipedObjects;
//
//    for (GeometricObject obj : clipedObjects) {
//
//        /* pega todos os pontos do objeto*/
//        std::vector<Point> objPoints = obj.getPointsVector();
//        /* pega o número de pontos do objeto*/
//        int objNumPoints = objPoints.size();
//
//        /* se tiver menos de dois pontos, quer dizer que não é possível desenhar
//         nem uma reta, no caso, o objeto seria apenas um ponto */
//
//        GeometricObject cliped = clipingDePoligono(objPoints);
//        clipedObjects.push_back(cliped);
//
//    }
//    return clipedObjects;
//}

double ViewPortTransformationX(double xw) {
    float xvp = ((xw - window.getXmin()) / (window.getXmax() - window.getXmin())) * (VIEW_PORT_WIDTH);
    return xvp;
}

double ViewPortTransformationY(double yw) {
    float yvp = (1 - ((yw - window.getYmin()) / (window.getYmax() - window.getYmin()))) * (VIEW_PORT_HEIGHT);
    return yvp;
}


//    /* MYSTEREOUS SIGNALS */
//    /* Signals used to handle the backing surface */
//    //g_signal_connect(viewPort, "draw", G_CALLBACK(draw_cb), NULL);
//    g_signal_connect(viewPort, "configure-event", G_CALLBACK(configure_event_cb), NULL);
//    /* Event signals */
//    g_signal_connect(viewPort, "motion-notify-event", G_CALLBACK(motion_notify_event_cb), NULL);
//    //g_signal_connect(viewPort, "button-press-event", G_CALLBACK(button_press_event_cb), NULL);
//    /* Ask to receive events the drawing area doesn't normally subscribe to. In particular, we need to ask for the
//     * button press and motion notify events that want to handle. */
//    gtk_widget_set_events(viewPort, gtk_widget_get_events(viewPort) | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
/* Redraw the screen from the surface. Note that the ::draw signal receives a ready-to-be-used 
 * cairo_t that is already clipped to only draw the exposed areas of the widget */
//static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
///* Handle motion events by continuing to draw if button 1 is still held down.The ::motion-notify 
// * signal handler receives a GdkEventMotion struct which contains this information. */
//static gboolean motion_notify_event_cb(GtkWidget *widget, GdkEventMotion *event, gpointer data);
//static void clear_surface(void);
///* Create a new surface of the appropriate size to store our scribbles */
//static gboolean configure_event_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer data);
///* Handle button press events by either drawing a rectangle or clearing the surface, depending on 
// * which button was pressed. The ::button-press signal handler receives a GdkEventButton
// * struct which contains this information. */
//static gboolean button_press_event_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);

//static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data) {
//    cairo_set_source_surface(cr, surface, 0, 0);
//    cairo_paint(cr);
//
//    return FALSE;
//}
//
//static void draw_brush(GtkWidget *widget, gdouble x, gdouble y) {
//    cairo_t *cr;
//
//    /* Paint to the surface, where we store our state */
//    cr = cairo_create(surface);
//
//    cairo_rectangle(cr, x - 3, y - 3, 6, 6);
//    cairo_fill(cr);
//
//    cairo_destroy(cr);
//
//    /* Now invalidate the affected region of the drawing area. */
//    gtk_widget_queue_draw_area(widget, x - 3, y - 3, 6, 6);
//}
//
//static gboolean motion_notify_event_cb(GtkWidget *widget, GdkEventMotion *event, gpointer data) {
//    /* paranoia check, in case we haven't gotten a configure event */
//    if (surface == NULL)
//        return FALSE;
//
//    if (event->state & GDK_BUTTON1_MASK)
//        draw_brush(widget, event->x, event->y);
//
//    /* We've handled it, stop processing */
//    return TRUE;
//}
//
//static gboolean configure_event_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
//    if (surface)
//        cairo_surface_destroy(surface);
//
//    surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget),
//            CAIRO_CONTENT_COLOR,
//            gtk_widget_get_allocated_width(widget),
//            gtk_widget_get_allocated_height(widget));
//
//    /* Initialize the surface to white */
//    clear_surface();
//
//    /* We've handled the configure event, no need for further processing. */
//    return TRUE;
//}
//
//static gboolean button_press_event_cb(GtkWidget *widget, GdkEventButton *event, gpointer data) {
//    /* paranoia check, in case we haven't gotten a configure event */
//    if (surface == NULL)
//        return FALSE;
//
//    if (event->button == GDK_BUTTON_PRIMARY) {
//        draw_brush(widget, event->x, event->y);
//    } else if (event->button == GDK_BUTTON_SECONDARY) {
//        clear_surface();
//        gtk_widget_queue_draw(widget);
//    }
//
//    /* We've handled the event, stop processing */
//    return TRUE;
//}
//
//struct Window
//{
//    GtkWidget *numerator;
//    GtkWidget *denominator;
//    GtkWidget *button;
//    GtkWidget *label;
//};
//
//
//void button_clicked(GtkWidget *widget, gpointer data)
//{
//    Window* w = (Window*)data;
//    char buf[10];
//
//    char buffer[200];
//
//    GtkEntry* e = (GtkEntry*)w->numerator;
//    const gchar* entry1 = gtk_entry_get_text(e);
//
//    char* test = (char*)entry1;
//    int r = atoi(test);
//
//
//    sprintf(buf,"%d",r);
//
//    GtkWidget *label = w->label;
//    gtk_label_set_text(GTK_LABEL(label), buf);
//}
//
//
//int main(int argc, char*argv[])
//{
//    GtkWidget *window;
//    GtkWidget *table;
//    Window w;
//
//
//    //Set up my window
//    gtk_init(&argc,&argv);
//    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//    gtk_window_set_title(GTK_WINDOW(window), "Division");
//    gtk_window_set_default_size(GTK_WINDOW(window),500,500);
//    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
//
//    //Create my table and add it to the window
//    table = gtk_table_new(4,2,FALSE);
//    gtk_container_add(GTK_CONTAINER(window),table);
//
//    //Create instances of all my widgets
//    w.numerator = gtk_entry_new();
//    w.denominator = gtk_entry_new();
//    w.button = gtk_button_new_with_label("Click");
//    w.label = gtk_label_new("result");
//
//    //Attack the widgets to the table
//    gtk_table_attach(GTK_TABLE(table), w.numerator,0,1,0,1,GTK_FILL,GTK_FILL,5,5);
//    gtk_table_attach(GTK_TABLE(table), w.denominator,0,1,1,2,GTK_FILL,GTK_FILL,5,5);
//    gtk_table_attach(GTK_TABLE(table), w.button,0,1,2,3,GTK_FILL,GTK_FILL,5,5);
//    gtk_table_attach(GTK_TABLE(table), w.label,0,1,3,4,GTK_FILL,GTK_FILL,5,5);
//
//    //attach the click action to with the button to invoke the button_clicked function
//    g_signal_connect(G_OBJECT(w.button),"clicked",G_CALLBACK(button_clicked),&w);   
//      g_signal_connect_swapped(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
//
//    gtk_widget_show_all(window);
//
//
//    gtk_main();
//
//    return 0;
//}
//#include <gtk/gtk.h>
//
//enum {
//    COL_NAME = 0,
//    COL_AGE,
//    NUM_COLS
//};
//
//static GtkTreeModel * create_and_fill_model(void) {
//    GtkListStore *store;
//    GtkTreeIter iter;
//
//    store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_UINT);
//
//    /* Append a row and fill in some data */
//    gtk_list_store_append(store, &iter);
//    gtk_list_store_set(store, &iter,
//            COL_NAME, "Heinz El-Mann",
//            COL_AGE, 51,
//            -1);
//
//    /* append another row and fill in some data */
//    gtk_list_store_append(store, &iter);
//    gtk_list_store_set(store, &iter,
//            COL_NAME, "Jane Doe",
//            COL_AGE, 23,
//            -1);
//
//    /* ... and a third row */
//    gtk_list_store_append(store, &iter);
//    gtk_list_store_set(store, &iter,
//            COL_NAME, "Joe Bungop",
//            COL_AGE, 91,
//            -1);
//
//    return GTK_TREE_MODEL(store);
//}
//
//static GtkWidget * create_view_and_model(void) {
//    GtkCellRenderer *renderer;
//    GtkTreeModel *model;
//    GtkWidget *view;
//
//    view = gtk_tree_view_new();
//
//    /* --- Column #1 --- */
//
//    renderer = gtk_cell_renderer_text_new();
//    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
//            -1,
//            "Name",
//            renderer,
//            "text", COL_NAME,
//            NULL);
//
//    /* --- Column #2 --- */
//
//    renderer = gtk_cell_renderer_text_new();
//    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
//            -1,
//            "Age",
//            renderer,
//            "text", COL_AGE,
//            NULL);
//
//    model = create_and_fill_model();
//
//    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
//
//    /* The tree view has acquired its own reference to the
//     *  model, so we can drop ours. That way the model will
//     *  be freed automatically when the tree view is destroyed */
//
//    g_object_unref(model);
//
//    return view;
//}
//
//int main(int argc, char **argv) {
//    GtkWidget *window;
//    GtkWidget *view;
//
//    gtk_init(&argc, &argv);
//
//    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//    g_signal_connect(window, "delete_event", gtk_main_quit, NULL); /* dirty */
//
//    view = create_view_and_model();
//
//    gtk_container_add(GTK_CONTAINER(window), view);
//
//    gtk_widget_show_all(window);
//
//    gtk_main();
//
//    return 0;
//}