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

    // Гол элемент

    r = r - 1;
    s = s - 1;

    Fraction a_rs = A_init[r][s];
    cout << "Гол элемент: (" << r + 1 << " мөр," << s + 1<< " багана) = " << a_rs << endl;

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

void gauss_jordan_2nd(
    Fraction **A, 
    int rows, 
    int cols, 
    vector<string> &coeff_array,
    int r,
    int c
    ) 
{
  // b-г Fraction-с string-рүү хувиргах
  vector<string> b_string;
  
  for (int i = 0; i < rows; i++)
  {
    ostringstream oss;
    oss << "y" << i + 1;
    b_string.push_back(oss.str());
  }

  // Анхны систем
  printSystem(
      A, b_string, coeff_array,
      rows, cols
  );

  Fraction a_rs = A[r - 1][c - 1];
  
  // Гол элэмент 0-тэй тэнцэж болохгүй
  if(a_rs.numerator != 0) 
  {
    cout << "Гол элемент: " << a_rs << endl;
    // cout << "Coeff array: " << coeff_array[j] << endl;
    // cout << "b_string array: " << b_string[r] << endl;

    // Гол элемент (r, j) 
    jordan_step(A, rows, cols, r, c);

    cout << "  Хувиргалтын дараахь матриц: " << endl;
          
    printSystem(
        A, b_string, coeff_array,
        rows, cols
    );
  }
  else
  {
    cout << "Гол элемент 0 байна." << endl;
  }
}

int main() 
{
    cout << "Мөрийн элементүүдийг зайгаар тусгаарлан оруулна уу." << endl;
    cout << "Хоосон мөр оруулбал дууссан гэж үзнэ." << endl;

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
    
    cout << endl << "Хувиргалт хийх мөр болон баганы дугаар оруулна уу: " << endl;

    int r, c;

    scanf("%d %d", &r, &c);
    
    int rows = fractionMatrix.size();
    cout << "Уншсан мөрийн тоо: " << rows 
         << " болон баганы тоо: " << cols << endl;

    bool negative;
    cout << "Матрицын элементүүдийг сөргөөр оруулсан уу? (1 - Тийм, 0 - Үгүй): " << endl;
    int input;
    cin >> input;
    negative = (input != 0);
         

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
            if (!negative)
            {
                A[i][j] = Fraction(
                    -fractionMatrix[i][j].numerator, 
                    fractionMatrix[i][j].denominator);
            }
            else
            {
                A[i][j] = Fraction(
                    fractionMatrix[i][j].numerator, 
                    fractionMatrix[i][j].denominator);
            }
        }
    }

    cout << endl << "A матриц: " << endl;

    printMatrix(A, rows, cols);

    // Гаусс-Жорданы 2-р хувиргалт
    gauss_jordan_2nd(
        A, rows, cols, 
        coeff_array, 
        r, c
    );

    // Санах ойгоо цэвэрлэх
    for(int i = 0; i < rows; i++)
    {
        delete[] A[i];
    }
    delete[] A;

    return 0;
}