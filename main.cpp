#include <iostream>
#include <vector>
#include "fraction.h"
#include <sstream>
#include <string>
#include <iomanip>
using namespace std;

string fractionToString(const Fraction &f) {
    std::ostringstream oss;
    oss << f;
    return oss.str();
}

void printMatrix(
    Fraction **A, 
    int rows,
    int cols
    ) 
{
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            cout << A[i][j] << "  ";
        }
        cout << endl;
    }
    cout << endl;
}

string center(const string &s, int width) 
{
    int len = s.size();

    if(len >= width) return s;
    int left = (width - len) / 2;
    int right = width - len - left;
    return string(left, ' ') + s + string(right, ' ');
}

void printSystem(
    Fraction **A, 
    vector<string> &b_string,
    vector<string> &coeff_array,
    // vector<int> &chosen_r,
    // vector<int> &chosen_c,
    int rows, 
    int cols
    ) 
{
    const int COL_WIDTH = 6;
    // cout << endl << "Coeff : " << endl;
    // for (auto &coeff : coeff_array)
    // {
    //     cout << coeff <<  "  ";
    // }
    // cout << endl;

    // Толгой хэсгийн мөрийг хэвлэх
    cout << center("", COL_WIDTH) << " || ";
    for (auto &coeff: coeff_array)
    {
        cout << center(coeff, COL_WIDTH) << " || ";
    }

    cout << endl;

    for(int i = 0; i < rows; i++)
    {
        cout << center(b_string[i], COL_WIDTH) << " || ";

        for(int j = 0; j < cols; j++)
        {
            ostringstream oss;
            oss << A[i][j];
            cout << center(oss.str(), COL_WIDTH) << " || ";
        }
        cout << endl;
    }
    cout << endl;
}

void jordan_step(
    Fraction **A, 
    int rows, 
    int cols,
    int r, 
    int s
    ) 
{
    // Эх матрицыг хадгалах
    Fraction **A_init = new Fraction*[rows];

    for(int i = 0; i < rows; i++)
    {
        A_init[i] = new Fraction[cols];
        for(int j=0; j<cols; j++)
        {
            A_init[i][j] = A[i][j];
        }
    }

    Fraction a_rs = A_init[r][s];
    cout << "Гол элемент: (" << r + 1 << " мөр," << s + 1 << " багана) = " << a_rs << endl;

    // ГОЛ ЭЛЕМЕНТ олох: 
    // b_rs = A[r][s] = 1 / -a_rs
    Fraction one(1,1);
    A[r][s] = one / a_rs;

    // Гол элементийн МӨРИЙН дагуу хувиргалт хийх
    for (int j = 0; j < cols; j++)
    {
        if(j != s)
        {
            A[r][j] = A[r][j] / a_rs;
        }
    }

    // Гол элементийн БАГАНЫ дагуу хувиргалт хийх
    Fraction minusOne(-1,1);
    for (int i = 0; i < rows; i++)
    {
        if(i != r)
        {
            A[i][s] = (A[i][s] * minusOne) / a_rs;
        }
    }

    // ҮЛДСЭН ЭЛЕМЕНТҮҮД дээр хувиргалт хийх
    // A[i][j] = (A_init[i][j]*a_rs - A_init[i][s]*A_init[r][j]) / a_rs for i!=r,j!=s
    for(int i = 0; i < rows; i++)
    {
        if(i != r)
        {
            for(int j = 0; j < cols; j++)
            {
                if(j != s)
                {
                    // A_init[i][j]*a_rs
                    Fraction term1 = A_init[i][j] * a_rs;

                    // A_init[i][s]*A_init[r][j]
                    Fraction term2 = A_init[i][s] * A_init[r][j];

                    A[i][j] = (term1 - term2) / a_rs;
                }
            }
        }
    }

    // Санах ойгоо цэвэрлэх
    for(int i = 0; i < rows; i++)
    {
        delete[] A_init[i];
    }
    delete[] A_init;
}

void find_solutions(
    Fraction **A,
    int rows,
    int cols,
    vector<int> &chosen_r,
    vector<int> &chosen_c,
    const vector<Fraction> &b,
    vector<string> &solutions,
    bool is_chosen_and_is_consistent,
    bool is_consistent
    )
{
    if (is_consistent)
    {
        // Шийд олох
        // b_init = copy of b
        vector<Fraction> b_init = b;
        cout << "b_init: " << endl;

        for(auto &val : b_init) cout << val << " ";
        cout << endl;

        // b_new
        Fraction minusOne(-1,1);
        vector<Fraction> b_new(rows, minusOne);

        /*
        хувиргалт хийх явцад үл мэдэгдэгч нар болон
        тогтмол тоонууд хэддүгээр мөр болон баганад орсныг
        дараах томъёогоор олов.

        b_new[ chosen_r[i] ] = b_init[ chosen_c[i] ]

        */
        int chosen_count = (int)chosen_r.size();

        for(int i = 0; i < chosen_count; i++)
        {
            b_new[ chosen_r[i] ] = b_init[ chosen_c[i] ];
        }

        cout << "b_new өөрчлөлт: " << endl;
        for(auto &val : b_new) cout << val << " ";
        cout << endl;

        // ============== ШИЙДҮҮДИЙГ ОЛОХ ===============

        cout << endl;
        cout << "Шийдүүдийг тооцоолж байна..\n";
        cout << endl;
        
        // Чөлөөт хувьсагч нарыг илрүүлэх
        vector<int> free_cols;
        for (int j = 0; j < cols; j++)
        {
            bool isChosen = false;
            for (int i = 0; i < chosen_count; i++)
            {
                if (chosen_c[i] == j)
                {
                    isChosen = true;
                    break;
                }
            }
            if (!isChosen)
            {
                free_cols.push_back(j);
            }
        }

        // cout << endl << "Чөлөөт үл мэдэгдэгчдийг хэвлэх: " << endl;
        // for (auto &col : free_cols) cout << col << " ";
        // cout << endl;

        // Сүүлчийн шийдүүдийг хадгалах
        vector<Fraction> fraction_exact_solutions;
        fraction_exact_solutions.assign(rows, Fraction(0, 1));

        vector<vector<string>> parametric_solutions;


        if (!is_chosen_and_is_consistent)
        {
            
            // Шийдүүдийг чөлөөт үл мэдэгдэгчүүдийг оруулахгүйгээр олох
            // cout << " HEllo " << endl;
            for(int i = 0; i < (int)chosen_r.size(); i++)
            {
                Fraction sumFrac(0, 1);
                for(int j = 0; j < (int)chosen_c.size(); j++)
                {
                    // b_new[j] * A[i][j]
                    // cout << b_new[j] << endl;
                    // cout << A[i][j] << endl;
                    sumFrac = sumFrac + (b_new[j] * A[i][j]);
                }

                // Хэрэв r = rows болоод бүрэн хувиргалтаа хийсэн ба
                // Чөлөөт хувьсагчуудыг 0-ээс авсан үеийн шийд
                fraction_exact_solutions[chosen_c[i]] = sumFrac;
                cout << i + 1 << " дугаар мөрийн нийлбэр = " << sumFrac 
                    << " => шийд: " << fraction_exact_solutions[chosen_c[i]] << endl;
            }
            
            // for(auto &j : chosen_r)
            // {
            //     cout << j << endl;
            // }

            // cout << endl;

            // for(auto &j : chosen_c)
            // {
            //     cout << j << endl;
            // }
            // cout << endl;

            if (!free_cols.empty())
            {
                // Чөлөөт хувьсагч нар олдсон үед буюу m < n нөхцөл

                cout << endl;
                cout << "Чөлөөт үл мэдэгдэгчүүд: " << endl;
                for (auto &val : free_cols) cout << "x" << val + 1 << " ";
                cout << endl;
                
                
                for (int i = 0; i < (int)fraction_exact_solutions.size(); i++)
                {
                    // int numerator = solutions[i].numerator;
                    // int denominator = solutions[i].denominator;

                    // parametric_solutions.push_back("x" + to_string(i + 1) + " = " + 
                    //     to_string(numerator) + "/" + to_string(denominator))

                    vector<string> rowExpr;

                    for (auto &j : free_cols)
                    {   
                        // int remaining_cols_idx = cols - (int)free_cols.size();

                        int coeff_numerator = A[i][j].numerator;
                        int coeff_denominator = A[i][j].denominator;

                        // cout << (-coeff_numerator) << "/" << coeff_denominator << endl;

                        rowExpr.push_back(
                            "(" +
                            to_string(-coeff_numerator) + "/" +
                            to_string(coeff_denominator) +
                            ")" +
                            " * x" + to_string(j + 1));

                        // cout << parametric_solutions[i][j] << " ";
                    }
                    parametric_solutions.push_back(rowExpr);
                }
            }
        }
        else
        {
            // r < rows ба m = n эсвэл m < n үеийн тохиолдол

            for(auto &i : chosen_r)
            {
                Fraction sumFrac(0, 1);
                for(auto &j : chosen_c)
                {
                    // b_new[j] * A[i][j]
                    sumFrac = sumFrac + (b_new[j] * A[i][j]);
                }

                // Хэрэв r = rows болоод бүрэн хувиргалтаа хийсэн ба
                // Чөлөөт хувьсагчуудыг 0-ээс авсан үеийн шийд
                fraction_exact_solutions[i] = sumFrac;
                cout << i << " дугаар мөр, " << " нийлбэр = " << sumFrac 
                    << " => шийд " << fraction_exact_solutions[i] << endl;
            }

            cout << endl;
            cout << "Чөлөөт үл мэдэгдэгчүүд: " << endl;
            for (auto &val : free_cols) cout << "x" << val + 1 << " ";
            cout << endl;
            
            
            for (int i = 0; i < (int)fraction_exact_solutions.size(); i++)
            {
                vector<string> rowExpr;

                for (auto &j : free_cols)
                {   
                    // int remaining_cols_idx = cols - (int)free_cols.size();

                    int coeff_numerator = A[i][j].numerator;
                    int coeff_denominator = A[i][j].denominator;

                    // cout << (-coeff_numerator) << "/" << coeff_denominator << endl;

                    rowExpr.push_back(
                        "(" +
                        to_string(-coeff_numerator) + "/" +
                        to_string(coeff_denominator) +
                        ")" +
                        " * x" + to_string(j + 1));

                    // cout << parametric_solutions[i][j] << " ";
                }
                parametric_solutions.push_back(rowExpr);
            }
        }

        solutions.resize(rows);

        for (int i = 0; i < rows; i++)
        {
            string fracStr = fractionToString(fraction_exact_solutions[i]);

            string paramExpr;
            if (i < (int)parametric_solutions.size()) 
            {
                for (size_t k = 0; k < parametric_solutions[i].size(); k++) 
                {
                    if(k > 0) paramExpr += " + ";
                    paramExpr += parametric_solutions[i][k];
                }
            }

            if(!paramExpr.empty()) 
            {
                solutions[i] = fracStr + " + " + paramExpr;
            } 
            else 
            {
                solutions[i] = fracStr;
            }
        }
    }
}

void gauss_jordan_2nd(
    Fraction **A, 
    int rows, 
    int cols, 
    const vector<Fraction> &b, 
    vector<string> &solutions,
    vector<string> &coeff_array,
    bool is_consistent,
    vector<int> &chosen_c_pos
    ) 
{
    // Гол элементийн сонгогдсон тухайн мөр болон баганы дугаарыг хадгалах 2 хүснэгт
    vector<int> chosen_r;
    vector<int> chosen_c;

    // Систем нийцтэй байгаад ерөнхий шийдтэй эсэх
    bool is_chosen_and_is_consistent;

    int r = 0;

    // Тооцооллоо оновчтой болгох үүднээс болон системийн нийцтэй эсэхийг тооцоход
    // ашиглагдах, сонгох боломжтой баганы дугааруудыг хадгалах
    vector<int> canditate_cols;

    vector<string> b_string;

    for (auto &elem : b)
    {
        ostringstream oss;
        oss << elem;
        b_string.push_back(oss.str());

        // if (elem.denominator == 1)
        // {
        //     b_string.push_back(to_string(elem.numerator));
        // }
        // else
        // {
        //     b_string.push_back(to_string(elem.numerator) + "/" + to_string(elem.denominator));
        // }
    }

    // Сонгогдох боломжтой баганууд буюу үл мэдэгдэгч нарыг цуглуулах
    for (int i = 0; i < cols; i++)
    {
        canditate_cols.push_back(i);
    }

    // Анхны систем
    printSystem(
        A, b_string, coeff_array,
        rows, cols
    );

    // Нийт "боломжтой" хувиргалтын тоо нийт мөрний тоотой тэнцүү байна
    while(r < rows) 
    {
        bool is_chosen = false;

        for(auto &j : canditate_cols)
        {
            Fraction a_rs = A[r][j];
            
            // Гол элэмент 0-тэй тэнцэж болохгүй
            if(a_rs.numerator != 0) 
            {
                // Өмнө нь гол элемент сонгогдсон мөр болон багана байж болохгүй
                bool is_used = false;

                for(size_t k = 0; k < chosen_r.size(); k++)
                {
                    // Зэрэг (мөр, багана) байдлаар хувиргалт бүрт сонгож буй тул
                    // chosen_r, chosen_c - ээс зэрэг хайх ёстой.
                    if(chosen_r[k] == r || chosen_c[k] == j)
                    {
                        is_used = true;
                        break;
                    }
                }
                // Сонгогдоогүй байвал хувиргалтын үйлдэнэ
                if(!is_used)
                {
                    // Сонгосон мөр болон баганыг хадгалах
                    // Энэ нь дараа нь шийд болон системийг дэлгэцлэхэд чухал тустай
                    chosen_r.push_back(r);
                    chosen_c.push_back(j);
                    
                    string temp_elem;

                    temp_elem = coeff_array[j];
                    coeff_array[j] = b_string[r];
                    b_string[r] = temp_elem;

                    // cout << "Coeff array: " << coeff_array[j] << endl;
                    // cout << "b_string array: " << b_string[r] << endl;

                    // Гол элемент (r, j) 
                    jordan_step(A, rows, cols, r, j);

                    cout << r + 1 << "  Хувиргалтын дараахь матриц: " << endl;
                    
                    printSystem(
                        A, b_string, coeff_array,
                        rows, cols
                    );
                    // printMatrix(A, rows, cols);
                    
                    // Хувиргалт амжилттай хийгдсэн тул дараагийн мөрлүү шилжих
                    // Мөн сонгогдох боломжтой элементүүдээс 0 элемент байгаагүйг тэмдэглэх
                    r++;
                    is_chosen = true;
                    break;
                }
            }
        }
        if(!is_chosen) 
        {
            /*  
            Гол элемент олдоогүй гэдэг нь тухайн мөрөнд сонгох боломжтой
            гол элементүүд бүгд 0 гэсэн үг, эндээс систем нийцтэй эсвэл
            ерөнхий шийдтэй эсэх нь шийдэгдэнэ

            1) Хэрэв b[i = current_row] = b[i] * a[i][chosen_c] 
            бол нийцтэй ба ерөнхий шийдтэй буюу үлдсэн canditates баганууд нь 
            параметрүүд болно.

            2) Хэрэв тэнцэхгүй тохиолдолд систем нийцгүй.
            */
            
            // Бүх боломжит хувиргалтыг хийж чадаагүй үед
            if (r < rows)
            {
                Fraction left_side = b[r];

                Fraction right_side = Fraction(0, 1);
                
                for (auto &col : chosen_c)
                {
                    right_side = right_side + A[r][col]*b[col];
                }

                if (left_side != right_side)
                {
                    cout << "Систем нийцгүй, учир нь " << r + 1 << " мөр дээр хувиргалт хийхэд: "
                         << endl << "Зүүн талын утга: " << left_side << " != "
                         << "Баруун талын утга: " << right_side 
                         << endl << " тэнцэхгүй байна." << endl;

                    is_consistent = false;
                }
                else
                {
                    cout << "Зүүн талын утга: " << left_side << " == "
                         << "Баруун талын утга: " << right_side 
                         << endl << " тэнцэж буй тул " << endl 
                         <<"Систем ерөнхий шийдтэй." << endl;

                    cout << "Доорх үл мэдэгдэгч нар нь чөлөөт хувьсагч нар болно:" << endl;
                    for (auto &col : canditate_cols)
                    {
                        cout << col << " ";
                    }
                    cout << endl;

                    is_chosen_and_is_consistent = true;
                }

                // Хувиргалтыг зогсоох
                break;
            }
        }
    }

    for (int i = 0; i < (int)chosen_c.size(); i++)
    {
        chosen_c_pos.push_back(chosen_c[i]);
    }

    find_solutions(
        A, rows, cols, 
        chosen_r, chosen_c, 
        b, solutions, 
        is_chosen_and_is_consistent,
        is_consistent
    );
}

int main() 
{
    cout << "Мөрийн элементүүдийг зайгаар тусгаарлан оруулна уу." << endl;
    cout << "Хоосон мөр оруулбал дууссан гэж үзнэ." << endl;

    bool is_consistent = true;

    vector< vector<Fraction> > fractionMatrix;
    string line;
    int cols = -1;

    while (getline(cin, line))
    {
        // Автоматаар мөр болон баганы тоог оруулсан элементүүдээс уншина

        // Мөр бүрээр унших
        if (line.empty())
        {
            // Хоосон мөр оруулбал дууссан гэж үзнэ
            break;
        }
        
        vector<Fraction> row;

        // Мөрөнд оруулсан элементүүдийг унших
        stringstream ss(line);
        string token;

        while (ss >> token)
        {
            // "/" тэмдэгтээр тусгаарлагдсан тоо бол бутархай тоо
            size_t slashPos = token.find('/');
            if (slashPos != string::npos)
            {  
                // Бутархай бүтцээр бутархай тоог унших
                int num = stoi(token.substr(0, slashPos));
                int den = stoi(token.substr(slashPos + 1));
                row.push_back(Fraction(num, den));
            }
            else
            {
                // Бутархай бүтцээр бүхэл тоог унших
                int num = stoi(token);
                row.push_back(Fraction(num, 1));
            }
        }

        // Эхний мөрөнд оруулсан элементүүдээс баганы тоог автоматаар олох
        if (cols < 0)
        {
            cols = row.size();
        }
        else if (row.size() != static_cast<size_t>(cols))
        {
            cerr << "Нийцгүй баганы тоотой байна!" << endl;
            return 1;
        }
        fractionMatrix.push_back(row);
    }
    

    int rows = fractionMatrix.size();
    cout << "Уншсан мөрийн тоо: " << rows 
         << " болон баганы тоо: " << cols << endl;

    // Гаусс-Жорданы 2-р хувиргалтад бэлтгэж элементүүдийг сөрөг тэмдэгтэй болгох
    vector<string> coeff_array;

    for (int i = 0; i < cols; i++)
    {
        coeff_array.push_back("x" + to_string(i + 1));
    }

    cout << "Coeff Array : " << endl;
    for (auto &val : coeff_array) cout << val << " ";
    cout << endl;

    // 2-р хувиргалтанд бэлдэж элементүүдийг сөрөг болгох

    Fraction **A = new Fraction*[rows];

    for (int i = 0; i < rows; i++)
    {
        A[i] = new Fraction[cols];

        for (int j = 0; j < cols; j++)
        {
            A[i][j] = Fraction(
                -fractionMatrix[i][j].numerator, 
                fractionMatrix[i][j].denominator);
        }
    }

    // Fraction **A = new Fraction*[rows];
    // for(int i = 0; i < rows; i++)
    // {
    //     A[i] = new Fraction[cols];
    // }

    // cout << "А матрицын элементүүдийг мөр бүрээр оруулна уу: " << endl;

    // for(int i = 0; i < rows; i++)
    // {
    //     for(int j = 0; j < cols; j++)
    //     {
    //         int val;
    //         // cin >> val;
    //         val = -1 * tempMatrix[i][j];
    //         A[i][j] = Fraction(val, 1);
    //     }
    // }

    Fraction minusOne(-1,1);

    // b векторын утгуудыг унших
    vector<Fraction> b(rows);
    cout << "b векторыг унших (" << rows << " утга байна!): " << endl;

    for(int i = 0; i < rows; i++)
    {
        int val;
        cin >> val;
        b[i] = Fraction(val, 1);
    }

    // Гаусс-Жорданы 2-р хувиргалтад бэлтгэж элементүүдийг сөрөг тэмдэгтэй болгох
    for(int i = 0; i < rows; i++)
    {
        b[i] = b[i] * minusOne;
    }

    cout << endl << "A матриц: " << endl;

    printMatrix(A, rows, cols);

    cout << "b: " << endl;
    for(auto &val : b) cout << val << " ";
    cout << endl << endl;

    // Шийдүүдийг бутархай бүтцээр хадгадах
    vector<string> solutions;
    vector<int> chosen_c_pos;

    // Гаусс-Жорданы 2-р хувиргалт
    gauss_jordan_2nd(
        A, rows, cols, 
        b, solutions, coeff_array, 
        is_consistent, chosen_c_pos
    );

    // cout << endl;
    // for (int i = 0; i < (int)chosen_c_pos.size(); i++)
    // {
    //     cout << chosen_c_pos[i] << " ";
    // }

    cout << endl;

    cout << endl << "Шийдүүд: " << endl;
    for(size_t i = 0; i < solutions.size(); i++)
    {
        cout << "x" << i + 1 << " = " << solutions[i] << endl;
    }
    cout << endl;

    // if (is_consistent)
    // {
    //     if (!chosen_c_pos.empty() && chosen_c_pos[0] == 0)
    //     {
    //         cout << endl << "Шийдүүд: " << endl;
    //         for(size_t i = 0; i < solutions.size(); i++)
    //         {
    //             cout << "x" << i + 1 << " = " << solutions[i] << endl;
    //         }
    //         cout << endl;
    //     }
    //     else
    //     {
    //         cout << " Hello " << endl;
    //         cout << endl << "Шийдүүд: " << endl;
    //         for(int i = 0; i < (int)chosen_c_pos.size(); i++)
    //         {
    //             cout << "x" << i + 1
    //                  << " = " << solutions[chosen_c_pos[i]] << endl;
    //         }
    //         cout << endl;
    //     }
    // }

    // Санах ойгоо цэвэрлэх
    for(int i = 0; i < rows; i++)
    {
        delete[] A[i];
    }
    delete[] A;

    return 0;
}