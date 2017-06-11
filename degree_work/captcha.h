#ifndef CAPTCHA_H
#define CAPTCHA_H

#include <QObject>
#include <QImage>
#include <QPainter>

#define white qRgb(255,255,255)
#define black qRgb(0,0,0)
#define red qRgb(255,0,0)
#define light_green qRgb(0,127,0)
#define green qRgb(0,255,0)
#define blue qRgb(0,0,255)
#define aqua qRgb(0, 255, 255)
#define purple qRgb(255, 0, 255)
#define gray qRgb(127, 127, 127)

#define draw_size_coef 0.02
#define draw_scale 5


// Вершина ------------------------------------------------------------------
struct N {
    int x; // координаты
    int y;
    // номера исходящих рёбер в векторе
    QVector<int>links;

    N();
    N(int a, int b);
    bool operator == (const N & n);
};

// Ребро --------------------------------------------------------------------
struct E {
    int begin; // начальная вершина
    int end; // конечная
    double thickness; // толщина
    double b_x; // вектора втыкания
    double b_y;
    double e_x;
    double e_y;
    double angle_ox; // от 0 до 1
    QVector<int>X; // тут массив точек - упорядоченных как ребро
    QVector<int>Y; // только для толщины и производных

    E();
    E(int index_of_start_node, int index_of_finish_node);
    void NormaliseDerivativeVector();
    E & operator = (const E & e);
    E operator-();
    E operator+(const E & e);
};

// Капча --------------------------------------------------------------------
class Captcha : public QObject
{
    QRgb** pix;
    int width;
    int height;
    int phase;

    QVector<N>Nodes;
    QVector<E>Edges;

// ThinLines: ----------
    int DeleteRedLayer(const QVector<int>& layer_x, const QVector<int>& layer_y); // снимаем один слой
    int DeleteRedPoint(int x, int y, int from_x, int from_y); // удаляем точку, если она не связующая
// ToGraph: ------------
    void GoToNextNode(int x, int y, int from_node);
    bool AddNode(const N &n);
    bool AddNode(int x, int y);
    bool AddEdge(E &e);
    bool AddEdge(int index_of_start_node, int index_of_finish_node);
    bool NewEdgeFromNodes(const N &from, const N &to);
    bool DeleteNode(int index_of_node);
    bool ExtendNode(int Number);
    int SearchNumberOfNode(int x, int y);
    bool DeleteEdge(int index_of_start_edge);
    double AngleEdges(int i, int j);
// Simplify: -----------
    void SimplifyNeighborNodes(QVector<int>&save_to, int i);
// ---------------------
    Q_OBJECT
public:
    explicit Captcha(const QImage&); // phase = 0
    void MonoChrome(); // phase = 1
    void ThinLines(); // phase = 2
    void ToGraph(); // phase = 3
    void Simplify(); // phase = 4
    void DeleteShortEdges(); // phase = 5
    void AddTurnNodes(); // phase = 6
    void DifGet(); // phase = 7
    void ShadowEdges(); // phase = 8
    void GetAngles(); // phase = 9
// Вывод: --------------
    QImage DrawImage();
    QImage DrawGraph();
    QString PrintGraph();
// Параметры
    int bw_critery;
    bool draw_sign;
    int minimal_length_of_edge;
    int shadow_edge_search_radius;
    double min_angle_full;
    double min_angle_moment;

// ---------------------
    ~Captcha();
};

#endif // CAPTCHA_H
