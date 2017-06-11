#include "captcha.h"
#include "math.h"
#include <QDebug>

// -------------------------------------------------------------------
double length(QPointF a) {
    return sqrt(QPointF::dotProduct(a,a));
}
double angle(QPointF a) {
    double ac = acos(a.x()/length(a))*180/M_PI;
    return (a.y() >= 0)?ac:-ac;
}
// -------------------------------------------------------------------
N::N() {
    x = 0;
    y = 0;
}
N::N(int a, int b) {
    x = a;
    y = b;
}
bool N::operator == (const N & a) {
    return (x == a.x && y == a.y);
}
// -------------------------------------------------------------------
E::E() {
    begin = -1;
    end = -1;
    thickness = 1;
    b_x = 0;
    b_y = 0;
    e_x = 0;
    e_y = 0;
    angle_ox = 0;
}
E::E(int start, int finish) {
    begin = start;
    end = finish;
    thickness = 1;
    b_x = 0;
    b_y = 0;
    e_x = 0;
    e_y = 0;
    angle_ox = 0;
}
void E::NormaliseDerivativeVector() {
    if ((b_x == 0 && b_y == 0) || (e_x == 0 && e_y == 0))
        return;
    double b_x_copy = b_x;
    double b_y_copy = b_y;
    double e_x_copy = e_x;
    double e_y_copy = e_y;
    b_x /= sqrt(b_x_copy * b_x_copy + b_y_copy * b_y_copy);
    b_y /= sqrt(b_x_copy * b_x_copy + b_y_copy * b_y_copy);
    e_x /= sqrt(e_x_copy * e_x_copy + e_y_copy * e_y_copy);
    e_y /= sqrt(e_x_copy * e_x_copy + e_y_copy * e_y_copy);
}
E&   E::operator = (const E & a) {
    if (this != &a) {
        begin = a.begin;
        end = a.end;
        thickness = a.thickness;
        b_x = a.b_x;
        b_y = a.b_y;
        e_x = a.e_x;
        e_y = a.e_y;
        angle_ox = a.angle_ox;
        X = a.X;
        Y = a.Y;
    }
    return *this;
}
E E:: operator -() {
    E edge;
    edge.begin = end;
    edge.end = begin;
    edge.thickness = thickness;
    edge.b_x = e_x;
    edge.b_y = e_y;
    edge.e_x = b_x;
    edge.e_y = b_y;
    if (angle_ox > 0) edge.angle_ox = angle_ox - 180;
    else edge.angle_ox = angle_ox + 180;
    for (int i = (int)X.size() - 1; i >= 0; i--)
        edge.X.push_back(X[i]);
    for (int i = (int)Y.size() - 1; i >= 0; i--)
        edge.Y.push_back(Y[i]);
    return edge;
}
E E:: operator +(const E & a) {
    //if (end != a.begin || X[X.size() - 1] != a.X[0] || Y[Y.size() - 1] != a.Y[0]) qDebug() << "error!!!";
    E e;
    e.begin = begin;
    e.end = a.end;
    e.b_x = b_x;
    e.b_y = b_y;
    e.e_x = a.e_x;
    e.e_y = a.e_y;
    e.X = X << a.X;
    e.Y = Y << a.Y;
    e.thickness = (thickness * (int)X.size() + a.thickness * (int)a.X.size()) /
            ((int)X.size() + (int)a.X.size());
    return e;
}
// -------------------------------------------------------------------
Captcha::Captcha(const QImage& img) {
    phase = 0;
    width = img.width() + 2;
    height = img.height() + 2;
    pix = new QRgb*[width];
    for (int x = 0; x < width; x++) {
        pix[x] = new QRgb[height];
        for (int y = 0; y < height; y++)
            pix[x][y] = white;
    }
    for (int y = 0; y < height - 2; y++) {
        QRgb *r = (QRgb*)img.scanLine(y);
        for (int x = 0; x < width - 2; x++)
            pix[x + 1][y + 1] = r[x];
    }

    // Параметры
    bw_critery = 50;//250;
    draw_sign = true;
    minimal_length_of_edge = 20;
    shadow_edge_search_radius = 40;
    min_angle_full = 120;
    min_angle_moment = 120;
}
Captcha::~Captcha() {
    for (int x = 0; x < width; x++) {
        delete[] pix[x];
    }
    delete[] pix;
    for (int i = 0; i < (int)Nodes.size(); i++)
        Nodes[i].links.clear();
    for (int i = 0; i < (int)Edges.size(); i++) {
        Edges[i].X.clear();
        Edges[i].Y.clear();
    }
    Nodes.clear();
    Edges.clear();
}
bool    Captcha::AddEdge(E & p) {
    // for (int i = 0; i < (int)Edges.size(); i++)
    // if (Edges[i] == p)
    // return 0;
    if (p.begin >= (int)Nodes.size() || p.end >= (int)Nodes.size())
        return 0;
    if (p.begin == -1) // пусть начало не пустое
        return 0;
    if (p.end == -1) {
        Edges.push_back(p);
        Nodes[p.begin].links.push_back((int)Edges.size() - 1);
        return 1;
    }
    /*if ((p.b_x == 0 && p.b_y == 0) || (p.e_x == 0 && p.e_y == 0)) {
        p.b_x = Nodes[p.end].x - Nodes[p.begin].x;
        p.b_y = Nodes[p.end].y - Nodes[p.begin].y;
        p.e_x = p.b_x;
        p.e_y = p.b_y;
    }*/
    p.NormaliseDerivativeVector();
    Edges.push_back(p);
    Nodes[p.begin].links.push_back((int)Edges.size() - 1);
    Nodes[p.end].links.push_back((int)Edges.size() - 1);
    return 1;
}
bool    Captcha::AddEdge(int start, int finish) {
    E p(start, finish);
    return AddEdge(p);
}
bool    Captcha::AddNode(const N & a) {
    if (SearchNumberOfNode(a.x, a.y) != -1 && !(a.x == 0 && a.y == 0))
        return 0;
    Nodes.push_back(a);
    return 1;
}
bool    Captcha::AddNode(int x, int y) {
    N a(x, y);
    return AddNode(a);
}
void    Captcha::AddTurnNodes() {
    if (phase < 5) DeleteShortEdges();
    phase = 6;
    const int size = minimal_length_of_edge/2;
    QVector<int>NeedDeleteEdge;
    double ax, ay, bx, by, tmp_alpha; // вектора a и b
    double min_alpha, min_j; // максимальный случай угол + итератор
    double now_angle; // текущая  суммарная производная
    const int step = 5; // пиксельный шаг для производной
    int dir; // правая(-1)/левая(1) пара
    for (int i = 0; i < (int)Edges.size(); i++)
        if ((int)Edges[i].X.size() > 2 * size) {
            min_alpha = 360;
            for (int t = size; t < (int)Edges[i].X.size() - size; t++) {
                ax = Edges[i].X[t - size] - Edges[i].X[t];
                ay = Edges[i].Y[t - size] - Edges[i].Y[t];
                bx = Edges[i].X[t + size] - Edges[i].X[t];
                by = Edges[i].Y[t + size] - Edges[i].Y[t];
                tmp_alpha =
                        180/M_PI*acos((ax * bx + ay * by) / sqrt(ax * ax + ay * ay) / sqrt
                                      (bx * bx + by * by));

                if (tmp_alpha < min_alpha) {
                    min_alpha = tmp_alpha;
                    min_j = t;
                }
            }
            now_angle = 0; // метод 2
            for (int j = step; j < Edges[i].X.size() - step; j += step) {
                ax = Edges[i].X[j] - Edges[i].X[j - step];
                ay = Edges[i].Y[j] - Edges[i].Y[j - step];
                bx = Edges[i].X[j + step] - Edges[i].X[j];
                by = Edges[i].Y[j + step] - Edges[i].Y[j];
                dir = (bool)(ax * by - bx * ay >= 0) * 2 - 1;
                now_angle +=
                        180/M_PI*dir * acos((ax * bx + ay * by) / sqrt(ax * ax + ay * ay) / sqrt
                                            (bx * bx + by * by));
                if (fabs(now_angle) >= min_angle_full || min_alpha < min_angle_moment) {
                    // Делаем излом
                    AddNode(Edges[i].X[min_j], Edges[i].Y[min_j]);
                    AddEdge(Edges[i].begin, (int)Nodes.size() - 1);
                    AddEdge((int)Nodes.size() - 1, Edges[i].end);
                    // делим старое ребро пополам
                    for (int k = 0; k < min_j; k++) Edges[Edges.size() - 2].X.push_back(Edges[i].X[k]);
                    for (int k = 0; k < min_j; k++) Edges[Edges.size() - 2].Y.push_back(Edges[i].Y[k]);
                    for (int k = min_j; k < Edges[i].X.size(); k++) Edges[Edges.size() - 1].X.push_back(Edges[i].X[k]);
                    for (int k = min_j; k < Edges[i].Y.size(); k++) Edges[Edges.size() - 1].Y.push_back(Edges[i].Y[k]);
                    NeedDeleteEdge.push_back(i);
                    break;
                }
            }
        }
    for (int i = 0; i < (int)NeedDeleteEdge.size(); i++) // удаляем
        DeleteEdge(NeedDeleteEdge[i] - i);
}
double  Captcha::AngleEdges(int i, int j){
    if (i == j || i < 0 || j < 0 || i >= Edges.size() || j >= Edges.size()) return 0;
    QPointF a, b;
    if (Edges[i].begin == Edges[j].begin) {
        a = QPointF(Edges[i].b_x, Edges[i].b_y);
        b = QPointF(Edges[j].b_x, Edges[j].b_y);
    }
    if (Edges[i].begin == Edges[j].end) {
        a = QPointF(Edges[i].b_x, Edges[i].b_y);
        b = QPointF(Edges[j].e_x, Edges[j].e_y);
    }
    if (Edges[i].end == Edges[j].begin) {
        a = QPointF(Edges[i].e_x, Edges[i].e_y);
        b = QPointF(Edges[j].b_x, Edges[j].b_y);
    }
    if (Edges[i].end == Edges[j].end) {
        a = QPointF(Edges[i].e_x, Edges[i].e_y);
        b = QPointF(Edges[j].e_x, Edges[j].e_y);
    }
    return 180/M_PI*acos(QPointF::dotProduct(a,b)/length(a)/length(b));
}
bool    Captcha::DeleteEdge(int Number) {
    if (Number >= (int)Edges.size() || Number < 0)
        return 0;
    // удаляем указатели на рёбра
    for (int i = 0; i < (int)Nodes.size(); i++) {
        for (int j = 0; j < (int)Nodes[i].links.size(); j++) {
            if (Nodes[i].links[j] == Number) {
                Nodes[i].links.erase(Nodes[i].links.begin() + j);
                j--;
                continue;
            }
            if (Nodes[i].links[j] > Number)
                Nodes[i].links[j]--;
        }
    }
    Edges[Number].X.clear();
    Edges[Number].Y.clear();
    Edges.erase(Edges.begin() + Number);
    return 1;
}
bool    Captcha::DeleteNode(int Number) { // только если рёбра уже перенаправлены
    if (Number >= (int)Nodes.size() || Number < 0)
        return 0;
    for (int i = 0; i < (int)Edges.size(); i++)
        if (Edges[i].begin == Number || Edges[i].end == Number)
            return 0;
    Nodes[Number].links.clear(); // вдруг память не почиститься
    Nodes.remove(Number); // удаляем Node[Number]
    for (int i = 0; i < (int)Edges.size(); i++) {
        // сдвигаем указатели на вершины
        if (Edges[i].begin > Number)
            Edges[i].begin--;
        if (Edges[i].end > Number)
            Edges[i].end--;
    }
    return 1;
}
int     Captcha::DeleteRedLayer(const QVector<int>&X, const QVector<int>&Y) {
    int deleted = 0;
    DeleteRedPoint(X[0], Y[0], X[0], Y[0]);
    int LastX = X[0], LastY = Y[0];
    for (int i = 1; i < (int)X.size() - 1; i++) {
        // идём налево налево
        if (X[i + 1] - X[i] < 0 && X[i] - X[i - 1] < 0) {
            deleted += DeleteRedPoint(X[i], Y[i], LastX, LastY);
            LastX = X[i];
            LastY = Y[i];
        }
        // идём направо направо
        if (X[i + 1] - X[i] > 0 && X[i] - X[i - 1] > 0) {
            deleted += DeleteRedPoint(X[i], Y[i] - 1, LastX, LastY);
            LastX = X[i];
            LastY = Y[i] - 1;
        }
        // идём вниз вниз
        if (Y[i + 1] - Y[i] > 0 && Y[i] - Y[i - 1] > 0) {
            deleted += DeleteRedPoint(X[i], Y[i], LastX, LastY);
            LastX = X[i];
            LastY = Y[i];
        }
        // идём вверх вверх
        if (Y[i + 1] - Y[i] < 0 && Y[i] - Y[i - 1] < 0) {
            deleted += DeleteRedPoint(X[i] - 1, Y[i], LastX, LastY);
            LastX = X[i] - 1;
            LastY = Y[i];
        }
        // идём налево вверх
        if (X[i] - X[i - 1] < 0 && Y[i + 1] - Y[i] < 0) {
            deleted += DeleteRedPoint(X[i], Y[i], LastX, LastY);
            // Deleted += DeleteRed(X[i] - 1, Y[i], X[i], Y[i]);
            LastX = X[i] - 1;
            LastY = Y[i];
        }
        // идём налево вниз
        if (X[i] - X[i - 1] < 0 && Y[i + 1] - Y[i] > 0) {
            deleted += DeleteRedPoint(X[i], Y[i], LastX, LastY);
            LastX = X[i];
            LastY = Y[i];
        }
        // идём направо вверх - ничего
        // идём направо вниз
        if (X[i] - X[i - 1] > 0 && Y[i + 1] - Y[i] > 0) {
            // Deleted += DeleteRed(X[i], Y[i] - 1, LastX, LastY);
            deleted += DeleteRedPoint(X[i], Y[i], X[i], Y[i] - 1);
            LastX = X[i];
            LastY = Y[i];
        }
        // идём вверх налево - ничего
        // идём вверх направо
        if (Y[i] - Y[i - 1] < 0 && X[i + 1] - X[i] > 0) {
            deleted += DeleteRedPoint(X[i] - 1, Y[i], LastX, LastY);
            // Deleted += DeleteRed(X[i] - 1, Y[i] - 1, X[i] - 1,
            // Y[i]);
            deleted += DeleteRedPoint(X[i], Y[i] - 1, X[i] - 1, Y[i] - 1);
            LastX = X[i];
            LastY = Y[i] - 1;
        }
        // идём вниз налево
        if (Y[i] - Y[i - 1] > 0 && X[i + 1] - X[i] < 0) {
            // Deleted += DeleteRed(X[i], Y[i], LastX, LastY);
            LastX = X[i];
            LastY = Y[i];
        }
        // идём вниз направо - ничего
    }
    if (pix[X[0]][Y[0]] == white || pix[X[0]][Y[0]] == gray) { // независимость последнего от первого
        pix[X[0]][Y[0]] = black;
        deleted += DeleteRedPoint(X[(int)X.size() - 2], Y[(int)X.size() - 2],
                X[0], Y[0]);
        pix[X[0]][Y[0]] = gray;
    }
    return deleted;
}
int     Captcha::DeleteRedPoint(int to_i, int to_j, int from_i, int from_j) {
    // белый - удалён
    // чёрный - не удалим
    // красный - решаем судьбу
    // синий - предыдущая, если не удалили
    // аква - предыдущая, если удалили
    // пурпурный - если первый и удалили
    if (pix[to_i][to_j] != red)
        return 0;
    int deleted_points = 0;
    QRgb temp = pix[from_i][from_j];
    if (pix[from_i][from_j] == white || pix[from_i][from_j] == gray)
        pix[from_i][from_j] = aqua;
    // Последний удалённый не влияет
    pix[to_i][to_j] = blue;
    // белые вертикально
    if ((pix[to_i][to_j - 1] == white || pix[to_i][to_j - 1] == gray) && (pix[to_i][to_j + 1] == white || pix[to_i][to_j + 1] == gray))
        goto end; // белый и аква
    if (qGreen(pix[to_i][to_j - 1]) > 100 && qGreen(pix[to_i][to_j + 1]
                                                    // косяк
                                                    ) > 100 && (qGreen(pix[to_i - 1][to_j - 1]) == 0 ||
                                                                qGreen(pix[to_i - 1][to_j]) == 0 || qGreen(pix[to_i - 1][to_j +
                                                                                                           1]) == 0) && (qGreen(pix[to_i + 1][to_j - 1]) == 0 ||
                                                                                                                         qGreen(pix[to_i + 1][to_j]) == 0 || qGreen(pix[to_i + 1][to_j +
                                                                                                                                                                    1]) == 0))
        goto end;
    // белые горизонтально
    if ((pix[to_i - 1][to_j] == white || pix[to_i - 1][to_j] == gray) && (pix[to_i + 1][to_j] == white || pix[to_i + 1][to_j] == gray))
        goto end;
    if (qGreen(pix[to_i - 1][to_j]) > 100 && qGreen(pix[to_i + 1][to_j]
                                                    // косяк
                                                    ) > 100 && (qGreen(pix[to_i - 1][to_j - 1]) == 0 ||
                                                                qGreen(pix[to_i][to_j - 1]) == 0 || qGreen(pix[to_i + 1][to_j -
                                                                                                           1]) == 0) && qGreen((pix[to_i - 1][to_j + 1]) == 0 ||
                                                                                                                               qGreen(pix[to_i][to_j + 1]) == 0 || qGreen(pix[to_i + 1][to_j +
                                                                                                                                                                          1]) == 0))
        goto end;
    if (qGreen(pix[to_i - 1][to_j]) > 100 && qGreen(pix[to_i][to_j -
                                                    1]) > 100 && qGreen(pix[to_i - 1][to_j - 1]) == 0)
        // красный, чёрный, синий
        goto end;
    // угол право верх
    if (qGreen(pix[to_i + 1][to_j]) > 100 && qGreen(pix[to_i][to_j -
                                                    1]) > 100 && qGreen(pix[to_i + 1][to_j - 1]) == 0)
        goto end;
    // угол лево низ
    if (qGreen(pix[to_i - 1][to_j]) > 100 && qGreen(pix[to_i][to_j +
                                                    1]) > 100 && qGreen(pix[to_i - 1][to_j + 1]) == 0)
        goto end;
    // угол право низ
    if (qGreen(pix[to_i + 1][to_j]) > 100 && qGreen(pix[to_i][to_j +
                                                    1]) > 100 && qGreen(pix[to_i + 1][to_j + 1]) == 0)
        goto end;
    pix[to_i][to_j] = gray; // будут удалены
    deleted_points++;
end:
    if (from_i != to_i || from_j != to_j)
        pix[from_i][from_j] = temp; // возвращаем
    return deleted_points;
}
void    Captcha::DeleteShortEdges() {
    if (phase < 4) Simplify();
    phase = 5;
    bool del_one, del = false;
    QVector<N>need_delete_node;
    QVector<N>need_extend_node;
    for (int i = 0; i < Edges.size(); i++) {
        del_one = false;
        if (Edges[i].X.size() < minimal_length_of_edge) {
            if (Nodes[Edges[i].begin].links.size() == 1) {
                del_one = true;
                need_delete_node.push_back(Nodes[Edges[i].begin]);
                need_extend_node.push_back(Nodes[Edges[i].end]);
            } else if (Nodes[Edges[i].end].links.size() == 1) {
                del_one = true;
                need_delete_node.push_back(Nodes[Edges[i].end]);
                need_extend_node.push_back(Nodes[Edges[i].begin]);
            } else if (Edges[i].end == Edges[i].begin) {
                del_one = true;
                need_delete_node.push_back(Nodes[Edges[i].begin]);
            }
            if (del_one) {
                DeleteEdge(i);
                i--;
            }
        }
        del |= del_one;
    }
    if (del) {
        for (int i = 0; i < need_delete_node.size(); i++)
            DeleteNode(SearchNumberOfNode(need_delete_node[i].x, need_delete_node[i].y));
        // склеиваем
        for (int i = 0; i < need_extend_node.size(); i++)
            ExtendNode(SearchNumberOfNode(need_extend_node[i].x, need_extend_node[i].y));
        DeleteShortEdges();
    }
}
void    Captcha::DifGet() {
    if (phase < 6) AddTurnNodes();
    phase = 7;
    int size;
    for (int i = 0; i < Edges.size(); i++) {
        Edges[i].b_x = 0;
        Edges[i].b_y = 0;
        Edges[i].e_x = 0;
        Edges[i].e_y = 0;
        size = fmin(ceil(Edges[i].X.size() / 2.0), (width + height)/15);
        for (int j = 0; j < size; j++) {
            Edges[i].b_x += Edges[i].X[j] - Edges[i].X[0];
            Edges[i].b_y += Edges[i].Y[j] - Edges[i].Y[0];
            Edges[i].e_x += Edges[i].X[Edges[i].X.size() - j - 1] -
                    Edges[i].X[Edges[i].X.size() - 1];
            Edges[i].e_y += Edges[i].Y[Edges[i].Y.size() - j - 1] -
                    Edges[i].Y[Edges[i].Y.size() - 1];
        }
        Edges[i].NormaliseDerivativeVector();
    }
}
QImage  Captcha::DrawGraph(){
    QImage img((width - 2)*draw_scale, (height - 2)*draw_scale, QImage::Format_RGB32);
    img.fill(white);
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    double size_coef;
    if (width < height)
        size_coef = ceil(img.width()*draw_size_coef)/2;
    else
        size_coef = ceil(img.height()*draw_size_coef)/2;
    double b_x, b_y, e_x, e_y, dx, dy, dist, t;
    QPointF p0, p1, p2, p3, begin_arrow, end_arrow, sign_position;
    QPolygonF arrow;
    QFont f("Cambria", size_coef*1.5);
    for (int i = 0; i < Edges.size(); i++) {
        if (Edges[i].end == Edges[i].begin) continue;
        dx = Nodes[Edges[i].end].x - Nodes[Edges[i].begin].x;
        dy = Nodes[Edges[i].end].y - Nodes[Edges[i].begin].y;
        dist = sqrt(dx * dx + dy * dy);
        if (Edges[i].b_x == 0 && Edges[i].b_y == 0) {
            b_x = dx / dist;
            b_y = dy / dist;
        } else {
            b_x = Edges[i].b_x;
            b_y = Edges[i].b_y;
        }
        if (Edges[i].e_x == 0 && Edges[i].e_y == 0) {
            e_x = -dx / dist;
            e_y = -dy / dist;
        } else {
            e_x = Edges[i].e_x;
            e_y = Edges[i].e_y;
        }
        p0.setX(Nodes[Edges[i].begin].x);
        p0.setY(Nodes[Edges[i].begin].y);
        p1.setX(Nodes[Edges[i].begin].x + b_x * dist/3);
        p1.setY(Nodes[Edges[i].begin].y + b_y * dist/3);
        p2.setX(Nodes[Edges[i].end].x + e_x * dist/3);
        p2.setY(Nodes[Edges[i].end].y + e_y * dist/3);
        p3.setX(Nodes[Edges[i].end].x);
        p3.setY(Nodes[Edges[i].end].y);
        p0*=draw_scale;
        p1*=draw_scale;
        p2*=draw_scale;
        p3*=draw_scale;
        QPainterPath lines;
        lines.moveTo(p0);
        lines.cubicTo(p1,p2,p3);
        t = 0.5;
        begin_arrow = pow(1 - t, 3)*p0 + 3*t*pow(1-t,2)*p1 + 3*pow(t,2)*(1-t) * p2 + pow(t,3) * p3;
        t = 0.55;
        end_arrow = (pow(1 - t, 3)*p0 + 3*t*pow(1-t,2)*p1 + 3*pow(t,2)*(1-t) * p2 + pow(t,3) * p3) - begin_arrow;
        end_arrow *= size_coef/sqrt(pow(end_arrow.x(),2) + pow(end_arrow.y(),2));
        end_arrow += begin_arrow;
        arrow   << end_arrow
                << QPointF(-(end_arrow.y() - begin_arrow.y())/2 + begin_arrow.x(), (end_arrow.x() - begin_arrow.x())/2 + begin_arrow.y())
                << QPointF((end_arrow.y() - begin_arrow.y())/2 + begin_arrow.x(), -(end_arrow.x() - begin_arrow.x())/2 + begin_arrow.y())
                << end_arrow;
        painter.setPen(QColor(black));
        painter.setBrush(QBrush(white));
        painter.drawPath(lines);
        painter.setBrush(QBrush(black));
        painter.drawPolygon(arrow);
        arrow.clear();
        if (draw_sign) {
            QPainterPath letter;
            t = 0.4;
            sign_position = pow(1 - t, 3)*p0 + 3*t*pow(1-t,2)*p1 + 3*pow(t,2)*(1-t) * p2 + pow(t,3) * p3 - QPointF(size_coef/2, -size_coef*3/4) + 2* QPointF((end_arrow.y() - begin_arrow.y())/2, -(end_arrow.x() - begin_arrow.x())/2);
            letter.addText(sign_position, f, QString::number(i));
            painter.setPen(QColor(black));
            painter.setBrush(QBrush(blue));
            painter.drawPath(letter);
        }
    }
    for (int i = 0; i < Nodes.size(); i++) {
        painter.setPen(QColor(black));
        painter.setBrush(QBrush(red));
        QPointF point_center(Nodes[i].x,Nodes[i].y);
        point_center *= draw_scale;
        painter.drawEllipse(point_center, size_coef, size_coef);
        if (draw_sign) {
            QPainterPath letter;
            QPointF offset(1,1);
            if ((double)Nodes[i].x / width < 0.4) offset.setX(-1);
            //if ((double)Nodes[i].x / width > 0.6) offset.setX(1);
            if ((double)Nodes[i].y / height < 0.4) offset.setY(-1);
            //if ((double)Nodes[i].y / height > 0.6) offset.setY(1);
            sign_position = point_center + offset * 2 * size_coef  - QPointF(size_coef/2, -size_coef*3/4);
            if (sign_position.x() < size_coef) sign_position.setX(3/2*size_coef);
            if (sign_position.x() > img.width() - size_coef) sign_position.setX(img.width() - 5/2*size_coef);
            if (sign_position.y() < size_coef) sign_position.setY(11/4*size_coef);
            if (sign_position.y() > img.height() - size_coef) sign_position.setY(img.height() - 5/4*size_coef);
            letter.addText(sign_position, f, QString::number(i));
            painter.setBrush(QBrush(green));
            painter.drawPath(letter);
        }
    }
    return img;
}
QImage  Captcha::DrawImage(){
    QImage img(width - 2, height - 2, QImage::Format_RGB32);
    for (int y = 0; y < img.height(); y++) {
        QRgb *r = (QRgb*)img.scanLine(y);
        for (int x = 0; x < img.width(); x++)
            r[x] = pix[x + 1][y + 1];
    }
    return img;
}
bool    Captcha::ExtendNode(int Number){
    if (Number == -1 || Nodes[Number].links.size() != 2 || Nodes[Number].links[0] == Nodes[Number].links[1]) return false;
    E e, e0 = Edges[Nodes[Number].links[0]], e1 = Edges[Nodes[Number].links[1]];
    if (e0.begin == e1.begin) e = -e0 + e1;
    else if (e0.end == e1.end) e = e0 + (-e1);
    else if (e0.end == e1.begin) e = e0 + e1;
    else e = e1 + e0;
    AddEdge(e);
    if (Nodes[Number].links[0] > Nodes[Number].links[1]) {
        DeleteEdge(Nodes[Number].links[0]);
        DeleteEdge(Nodes[Number].links[0]);
    }
    else {
        DeleteEdge(Nodes[Number].links[1]);
        DeleteEdge(Nodes[Number].links[0]);
    }
    return DeleteNode(Number);
}
void    Captcha::GetAngles() {
    if (phase < 8) ShadowEdges();
    phase = 9;
    for (int i = 0; i < Edges.size(); i++)
        Edges[i].angle_ox = angle(QPointF(Nodes[Edges[i].end].x, -Nodes[Edges[i].end].y) - QPointF(Nodes[Edges[i].begin].x, -Nodes[Edges[i].begin].y));
}
void    Captcha::GoToNextNode(int i, int j, int FromNode) {
    bool start = false;
    if (Nodes[FromNode].x == i && Nodes[FromNode].y == j) { // в начале
        start = true;
    }
    if (pix[i][j] == black) // не вершина
        pix[i][j] = red;
    QRgb temp;
    for (int k = i - 1; k <= i + 1; k++)
        for (int l = j - 1; l <= j + 1; l++) {
            temp = pix[k][l];
            if (Edges.size() == 0)
                continue;
            if (temp == green && (SearchNumberOfNode(k, l) != FromNode ||
                                  (int)Edges[(int)Edges.size() - 1].X.size() > 9) && !start) {
                Edges[(int)Edges.size() - 1].end =
                        SearchNumberOfNode(k, l);
                // добавляем линк вручную
                Nodes[Edges[(int)Edges.size() - 1].end].links.push_back
                        ((int)Edges.size() - 1);
                Edges[(int)Edges.size() - 1].X.push_back(i);
                Edges[(int)Edges.size() - 1].Y.push_back(j);
                Edges[(int)Edges.size() - 1].X.push_back(k);
                Edges[(int)Edges.size() - 1].Y.push_back(l);
                return;
            }
        }
    for (int k = i - 1; k <= i + 1; k++)
        for (int l = j - 1; l <= j + 1; l++) {
            temp = pix[k][l];
            if (temp == black) {
                if (start) {
                    E edge;
                    edge.begin = FromNode;
                    AddEdge(edge);
                }
                Edges[(int)Edges.size() - 1].X.push_back(i);
                Edges[(int)Edges.size() - 1].Y.push_back(j);
                GoToNextNode(k, l, FromNode);
            }
        }
}
void    Captcha::MonoChrome() {
    phase = 1;
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            pix[x][y] = (qGray(pix[x][y])<bw_critery?black:white);
}
bool    Captcha::NewEdgeFromNodes(const N & a, const N & b) {
    E n;
    int beg = SearchNumberOfNode(a.x, a.y);
    int end = SearchNumberOfNode(b.x, b.y);
    n.begin = beg;
    n.end = end;
    n.thickness = 1;
    n.b_x = Nodes[end].x - Nodes[beg].x;
    n.b_y = Nodes[end].y - Nodes[beg].y; ;
    n.e_x = n.b_x;
    n.e_y = n.b_y;
    n.NormaliseDerivativeVector();
    n.angle_ox = 0;
    // тут срочно переделать
    return true;
}
QString Captcha::PrintGraph() {
    QString out = "Nodes:\r\nNumber\tx\ty\tlinks";
    out += "\r\n*****************************************************************************";
    for (int i = 0; i < Nodes.size(); i++) {
        out += "\r\n" + QString::number(i) + '\t' + QString::number(Nodes[i].x) + '\t' +
                QString::number(Nodes[i].y) + '\t';
        for (int j = 0; j < (int)Nodes[i].links.size(); j++)
            out += QString::number(Nodes[i].links[j]) + " ";
        out.left(out.length()-1);
    }
    out += "\r\n*****************************************************************************";
    out += "\r\nEdges:\r\nNumber\tbegin\tend\tb_x\tb_y\te_x\te_y\tangle\tthickn\tlength";
    out += "\r\n*****************************************************************************";
    for (int i = 0; i < Edges.size(); i++) {
        out += "\r\n" + QString::number(i) + '\t' +
                QString::number(Edges[i].begin) + '\t' +
                QString::number(Edges[i].end) + '\t' +
                QString::number(Edges[i].b_x, 'g', 3) + '\t' +
                QString::number(Edges[i].b_y, 'g', 3) + '\t' +
                QString::number(Edges[i].e_x, 'g', 3) + '\t' +
                QString::number(Edges[i].e_y, 'g', 3) + '\t' +
                QString::number(Edges[i].angle_ox, 'g', 3) + '\t' +
                QString::number(Edges[i].thickness, 'g', 3) + '\t' +
                QString::number(Edges[i].X.size());
    }
    out += "\r\n*****************************************************************************";
    return out;
}
int     Captcha::SearchNumberOfNode(int x, int y) {
    N a(x, y);
    for (int i = 0; i < (int)Nodes.size(); i++)
        if (Nodes[i] == a)
            return i;
    return -1;
}
void    Captcha::ShadowEdges() {
    if (phase < 7) DifGet();
    phase = 8;
    int min_dist, min_iter, min_edge_mas_iter;
    bool min_type_node;
    int nodes_dist, node_circle_dist, node_edge_dist;
    bool begin_i, begin_min; // false = end
    QPointF shadow_circle_center_i;
    QPointF p0, p1, p2, p3;
    for (int i = 0; i < Nodes.size(); i++)
        if (Nodes[i].links.size() == 1) {
            begin_i = (Edges[Nodes[i].links[0]].begin == i);
            if (begin_i) shadow_circle_center_i = QPointF(Nodes[i].x, Nodes[i].y) - shadow_edge_search_radius * QPointF(Edges[Nodes[i].links[0]].b_x, Edges[Nodes[i].links[0]].b_y);
            else shadow_circle_center_i = QPointF(Nodes[i].x, Nodes[i].y) - shadow_edge_search_radius * QPointF(Edges[Nodes[i].links[0]].e_x, Edges[Nodes[i].links[0]].e_y);
            min_iter = -1;
            min_dist = width+height;
            for (int j = 0; j < Nodes.size(); j++) {
                if (i == j) continue;
                nodes_dist = length(QPointF(Nodes[i].x,Nodes[i].y) - QPointF(Nodes[j].x,Nodes[j].y));
                node_circle_dist = length(shadow_circle_center_i - QPointF(Nodes[j].x,Nodes[j].y));
                if (node_circle_dist <= shadow_edge_search_radius && nodes_dist < min_dist) {
                    min_dist = nodes_dist;
                    min_iter = j;
                    min_type_node = true;
                }
            }
            for (int j = 0; j < Edges.size(); j++) {
                if (Edges[j].begin == i || Edges[j].end == i) continue;
                for (int k = 0; k < Edges[j].X.size(); k++) {
                    node_edge_dist = length(shadow_circle_center_i - QPointF(Edges[j].X[k],Edges[j].Y[k]));
                    if (node_edge_dist <= shadow_edge_search_radius && node_edge_dist < min_dist) {
                        min_dist = node_edge_dist;
                        min_iter = j;
                        min_edge_mas_iter = k;
                        min_type_node = false;
                    }
                }
            }
            if (min_iter > -1) {
                if (min_type_node) {
                    if (Nodes[min_iter].links.size() == 1) {
                        p0 = QPointF(Nodes[i].x, Nodes[i].y);
                        p3 = QPointF(Nodes[min_iter].x, Nodes[min_iter].y);
                        if (begin_i) p1 = p0 - min_dist/3 * QPointF(Edges[Nodes[i].links[0]].b_x, Edges[Nodes[i].links[0]].b_y);
                        else p1 = p0 - min_dist/3 * QPointF(Edges[Nodes[i].links[0]].e_x, Edges[Nodes[i].links[0]].e_y);
                        begin_min = (Edges[Nodes[min_iter].links[0]].begin == min_iter);
                        if (begin_min) p2 = p3 - min_dist/3 * QPointF(Edges[Nodes[min_iter].links[0]].b_x, Edges[Nodes[min_iter].links[0]].b_y);
                        else p2 = p3 - min_dist/3 * QPointF(Edges[Nodes[min_iter].links[0]].e_x, Edges[Nodes[min_iter].links[0]].e_y);
                        double t = .5;
                        QPointF tmp = pow(1 - t, 3)*p0 + 3*t*pow(1-t,2)*p1 + 3*pow(t,2)*(1-t) * p2 + pow(t,3) * p3;
                        N n(tmp.x(), tmp.y());
                        AddNode(n);
                        AddEdge(i, Nodes.size()-1);
                        // dif
                        if (begin_i) {
                            Edges[Edges.size()-1].b_x = -Edges[Nodes[i].links[0]].b_x;
                            Edges[Edges.size()-1].b_y = -Edges[Nodes[i].links[0]].b_y;
                        }
                        else {
                            Edges[Edges.size()-1].b_x = -Edges[Nodes[i].links[0]].e_x;
                            Edges[Edges.size()-1].b_y = -Edges[Nodes[i].links[0]].e_y;
                        }
                        Edges[Edges.size()-1].e_x = Nodes[i].x - Nodes[Nodes.size()-1].x;
                        Edges[Edges.size()-1].e_y = Nodes[i].y - Nodes[Nodes.size()-1].y;
                        Edges[Edges.size()-1].NormaliseDerivativeVector();
                        //
                        AddEdge(min_iter, Nodes.size()-1);
                        // dif
                        if (begin_min) {
                            Edges[Edges.size()-1].b_x = -Edges[Nodes[min_iter].links[0]].b_x;
                            Edges[Edges.size()-1].b_y = -Edges[Nodes[min_iter].links[0]].b_y;
                        }
                        else {
                            Edges[Edges.size()-1].b_x = -Edges[Nodes[min_iter].links[0]].e_x;
                            Edges[Edges.size()-1].b_y = -Edges[Nodes[min_iter].links[0]].e_y;
                        }
                        Edges[Edges.size()-1].e_x = Nodes[min_iter].x - Nodes[Nodes.size()-1].x;
                        Edges[Edges.size()-1].e_y = Nodes[min_iter].y - Nodes[Nodes.size()-1].y;
                        Edges[Edges.size()-1].NormaliseDerivativeVector();
                        //
                        ExtendNode(fmax(i, min_iter));
                        ExtendNode(fmin(i, min_iter));
                        if (AngleEdges(Nodes[Nodes.size()-1].links[0], Nodes[Nodes.size()-1].links[1]) > min_angle_moment)
                            ExtendNode(Nodes.size() - 1);
                    } else {
                        if (begin_i) Edges[Nodes[i].links[0]].begin = min_iter;
                        else Edges[Nodes[i].links[0]].end = min_iter;
                        Nodes[min_iter].links << Nodes[i].links[0];
                        DeleteNode(i);
                    }
                } else {
                    qDebug() << min_iter << min_edge_mas_iter;
                    // Делаем излом
                    AddNode(Edges[min_iter].X[min_edge_mas_iter], Edges[min_iter].Y[min_edge_mas_iter]);
                    AddEdge(Edges[min_iter].begin, Nodes.size() - 1);
                    AddEdge(Nodes.size() - 1, Edges[min_iter].end);
                    // делим старое ребро пополам
                    for (int k = 0; k < min_edge_mas_iter; k++) Edges[Edges.size() - 2].X.push_back(Edges[min_iter].X[k]);
                    for (int k = 0; k < min_edge_mas_iter; k++) Edges[Edges.size() - 2].Y.push_back(Edges[min_iter].Y[k]);
                    for (int k = min_edge_mas_iter; k < Edges[min_iter].X.size(); k++) Edges[Edges.size() - 1].X.push_back(Edges[min_iter].X[k]);
                    for (int k = min_edge_mas_iter; k < Edges[min_iter].Y.size(); k++) Edges[Edges.size() - 1].Y.push_back(Edges[min_iter].Y[k]);
                    DeleteEdge(min_iter);
                }
                ShadowEdges();
                return;
            }
        }
}
void    Captcha::Simplify() {
    if (phase < 3) ToGraph();
    phase = 4;
    QVector< QVector<int> >Un;
    QVector<int>UnionI;
    bool exist;
    for (int i = 0; i < Nodes.size(); i++) {
        UnionI.clear();
        SimplifyNeighborNodes(UnionI, i);
        qSort(UnionI.begin(), UnionI.end());
        exist = false;
        for (int j = 0; j < Un.size(); j++) {
            if (UnionI == Un[j]) exist = true;
        }
        if (!exist) Un.push_back(UnionI);
    }
    QVector<N>NewNodes;
    for (int i = 0; i < Un.size(); i++) {
        N n;
        for (int j = 0; j < Un[i].size(); j++) {
            n.links << Nodes[Un[i][j]].links;
            n.x += Nodes[Un[i][j]].x;
            n.y += Nodes[Un[i][j]].y;
            for (int k = 0; k < n.links.size(); k++) {
                if (Edges[n.links[k]].begin == Un[i][j]) Edges[n.links[k]].begin = Nodes.size() + NewNodes.size();
                if (Edges[n.links[k]].end == Un[i][j]) Edges[n.links[k]].end = Nodes.size() + NewNodes.size();
            }
        }
        n.x /= Un[i].size();
        n.y /= Un[i].size();
        NewNodes << n;
    }
    Nodes << NewNodes;
    while (Nodes.size() > NewNodes.size())
        DeleteNode(0);
    for (int i = 0; i < Nodes.size(); i++)
        if (Nodes[i].links.size() == 2 && Nodes[i].links[0] != Nodes[i].links[1]) {
            ExtendNode(i);
            i--;
        }
}
void    Captcha::SimplifyNeighborNodes(QVector<int>&main, int i_node) {
    for (int k = 0; k < main.size(); k++) {
        if (main[k] == i_node)
            return;
    }
    main.push_back(i_node);
    int tmp;
    for (int i = Nodes[i_node].x - 1; i <= Nodes[i_node].x + 1; i++)
        for (int j = Nodes[i_node].y - 1; j <= Nodes[i_node].y + 1; j++) {
            tmp = SearchNumberOfNode(i, j);
            if ((i != Nodes[i_node].x || j != Nodes[i_node].y) && tmp != -1)
                SimplifyNeighborNodes(main, tmp);
        }
}
void    Captcha::ThinLines() {
    if (phase == 0) MonoChrome();
    phase = 2;
    // красим край в красный
    for (int x = 1; x < width - 1; x++)
        for (int y = 1; y < height - 1; y++)
            if (pix[x][y] == black && (pix[x][y - 1] == white || pix[x - 1][y] == white || pix[x + 1][y] == white || pix[x][y + 1] == white || pix[x][y - 1] == gray || pix[x - 1][y] == gray || pix[x + 1][y] == gray || pix[x][y + 1] == gray))
                pix[x][y] = red;
    // вектор "граничных" контуров
    QVector< QVector<int> > BigX;
    // последовательность граничных точек (не пикселей)
    QVector< QVector<int> > BigY;
    int nextX, nextY;
    QVector<int>WayX;
    QVector<int>WayY;
    for (int x = 1; x < width - 1; x++)
        for (int y = 1; y < height - 1; y++)
            if ((pix[x][y] == white || pix[x][y] == gray) && pix[x + 1][y] == red) {
                if (pix[x][y] == white) pix[x][y] = light_green;
                if (pix[x][y] == gray) pix[x][y] = green;// чтобы не наткнуться на тот же слой
                WayX.clear();
                WayY.clear();
                WayX.push_back(x + 1);
                WayY.push_back(y);
                nextX = x + 1;
                nextY = y + 1;
                while (nextX != x + 1 || nextY != y) {
                    WayX.push_back(nextX);
                    WayY.push_back(nextY);
                    if (pix[nextX - 1][nextY] == white && pix[nextX][nextY] == red)
                        pix[nextX - 1][nextY] = light_green;
                    if (pix[nextX - 1][nextY] == gray && pix[nextX][nextY] == red)
                        pix[nextX - 1][nextY] = green; // не наткнуться
                    // разобраться с предыдущим ходом - в натуре класс
                    if (WayX[(int)WayX.size() - 2] == nextX - 1 && WayY[(int)WayY.size() - 2] == nextY && pix[nextX]
                            [nextY] == red) {
                        // |1|0| \n |0|1|   исключение слева
                        nextY++;
                        continue;
                    }
                    if (WayX[(int)WayX.size() - 2] == nextX +
                            1 && WayY[(int)WayY.size() - 2] == nextY && pix[nextX -
                            1][nextY - 1] == red) {
                        // |1|0| \n |0|1|   исключение справа
                        nextY--;
                        continue;
                    }
                    if (WayX[(int)WayX.size() - 2] == nextX && WayY
                            [(int) WayY.size() - 2] == nextY +
                            1 && pix[nextX][nextY - 1] == red) {
                        // |0|1| \n |1|0|   исключение снизу
                        nextX++;
                        continue;
                    }
                    if (WayX[(int)WayX.size() - 2] == nextX && WayY
                            [(int) WayY.size() - 2] == nextY -
                            1 && pix[nextX - 1][nextY] == red) {
                        // |0|1| \n |1|0|   исключение сверху
                        nextX--;
                        continue;
                    }
                    if (pix[nextX - 1][nextY] == red &&
                            (pix[nextX - 1][nextY - 1] == white || pix[nextX - 1][nextY - 1] == gray ||
                             pix[nextX - 1][nextY - 1] == green || pix[nextX - 1][nextY - 1] == light_green)) {
                        // налево
                        nextX--;
                        continue;
                    }
                    if (pix[nextX][nextY - 1] == red &&
                            (pix[nextX][nextY] == white || pix[nextX][nextY] == gray ||
                             pix[nextX][nextY] == green || pix[nextX][nextY] == light_green)) { // направо
                        nextX++;
                        continue;
                    }
                    if (pix[nextX][nextY] == red &&
                            (pix[nextX - 1][nextY] == white || pix[nextX - 1][nextY] == gray ||
                             pix[nextX - 1][nextY] == green || pix[nextX - 1][nextY] == light_green)) { // вниз
                        nextY++;
                        continue;
                    }
                    if (pix[nextX - 1][nextY - 1] == red &&
                            (pix[nextX][nextY - 1] == white || pix[nextX][nextY - 1] == gray || pix[nextX][nextY -
                             1] == green || pix[nextX][nextY -
                             1] == light_green)) {
                        // вверх
                        nextY--;
                        continue;
                    }
                }
                WayX.push_back(x + 1);
                WayY.push_back(y);
                BigX.push_back(WayX);
                BigY.push_back(WayY);
            }
    for (int x = 1; x < width - 1; x++)
        for (int y = 1; y < height - 1; y++) {
            if (pix[x][y] == light_green) pix[x][y] = white; // обратно в белый
            if (pix[x][y] == green) pix[x][y] = gray; // обратно в серый
        }
    int Del = 0;
    for (int i = 0; i < (int)BigX.size(); i++) // собственно замут
        Del += DeleteRedLayer(BigX[i], BigY[i]);
    for (int x = 1; x < width - 1; x++)
        for (int y = 1; y < height - 1; y++) {
            if (pix[x][y] == blue)
                pix[x][y] = black; // вернули в исход
        }
    if (Del)
        ThinLines();
}
void    Captcha::ToGraph() {
    if (phase < 2) ThinLines();
    phase = 3;
    int degree; // разветвлённость
    for (int i = 1; i < width - 1; i++) // Ищем начало
        for (int j = 1; j < height - 1; j++)
            if (pix[i][j] == black) {
                degree = 0;
                for (int k = i - 1; k <= i + 1; k++)
                    for (int l = j - 1; l <= j + 1; l++)
                        if (pix[k][l] == black && (k != i || l != j))
                            degree++;
                if (degree != 2)
                    AddNode(i, j);
            }
    for (int i = 0; i < (int)Nodes.size(); i++) // красим вершины
        pix[Nodes[i].x][Nodes[i].y] = green;
    for (int i = 0; i < (int)Nodes.size(); i++) // красим рёбра
        GoToNextNode(Nodes[i].x, Nodes[i].y, i);
    for (int i = 0; i < (int)Edges.size(); i++) // удаляем неудачные рёбра
        if (Edges[i].begin == -1 || Edges[i].end == -1)
            DeleteEdge(i); // может ещё и вершину нулевую удалить?
    for (int i = 1; i < width - 1; i++)
        for (int j = 1; j < height - 1; j++)
            if (pix[i][j] == green || pix[i][j] == red)
                pix[i][j] = black;
    for(int i = 0; i < Nodes.size(); i++)
        if(Nodes[i].links.size() == 0) {
            DeleteNode(i);
            i--;
        }
}
