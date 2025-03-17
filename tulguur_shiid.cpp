#include <iostream>
#include <vector>
#include <tuple>
#include "fraction.h"
#include <sstream>
#include <string>
#include <iomanip>
using namespace std;

struct Surug_Entry {
  int index;
  bool isAllPositive;
};

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
    vector<string> &original_vars,
    vector<string> &free_vars,
    // vector<int> &chosen_r,
    // vector<int> &chosen_c,
    int rows, 
    int cols
    ) 
{
    const int COL_WIDTH = 6;
    // cout << endl << "Coeff : " << endl;
    // for (auto &coeff : free_vars)
    // {
    //     cout << coeff <<  "  ";
    // }
    // cout << endl;

    // Толгой хэсгийн мөрийг хэвлэх
    cout << center("", COL_WIDTH) << " || ";
    for (auto &coeff: free_vars)
    {
        cout << center(coeff, COL_WIDTH) << " || ";
    }

    cout << endl;

    for(int i = 0; i < rows; i++)
    {
        cout << center(original_vars[i], COL_WIDTH) << " || ";

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

void tulguur_shiid(
    Fraction **A, 
    int rows, 
    int cols, 
    vector<string> &free_vars,
    vector<string> &original_vars,
    vector<tuple<int, Fraction>> &sul_gishuud
    ) 
{
  // Анхны систем
  printSystem(
      A, original_vars, free_vars,
      rows, cols
  );
  
  // F мөр болон сул гишүүний баганыг сонголтоос хасав.
  int valid_rows = rows - 1;
  int valid_cols = cols - 1;

  bool tulguur_shiid_oldson = false;
  bool shiidgui_eseh = false;

  while (!tulguur_shiid_oldson)
  {
    // Сөрөг сул гишүүдийг хайх
    bool surug_sul_gishuun_oldson = false;
    
    // Сөрөг сул гишүүн байгаа мөрийн дугааруудыг хадгалах
    vector<Surug_Entry> surug_sg_murnuud;

    // Сөрөг сул гишүүдийг dictionary буюу vector<tuple> бүтцээр
    // хадгалсан ба эхний элемент нь мөрийн дугаарыг болон удаахь нь
    // Тухайн мөрөнд харгалзах сул гишүүний утгыг хадгална
    // Иймд харгалзах мөрийн дугаарыг хадгалахад хялбар болно.
    for (const auto & [index, val]: sul_gishuud)
    {
      // 0-с бага сөрөг сул гишүүнийг хайх
      if (val.numerator < 0)
      {
        surug_sg_murnuud.push_back({index, false}); 
      }
    }

    if (surug_sg_murnuud.empty())
    {
      // Ямар ч сөрөг сул гишүүн олдоогүй тул тулгуур шийд олдсон
      tulguur_shiid_oldson = true;
      break;
    }
    else
    {
      // Сөрөг сул гишүүн олдсон тул үргэлжлүүлэх
      // Гол баганыг сонгох сөрөн сул гишүүд олдсон мөрийн дугааруудаас сонгох
      for (auto & surug_entry: surug_sg_murnuud)
      {
        // Тухайн сөрөг сул гишүүн байгаа мөрөнд орших элементүүд бүгд эерэг бол
        // Хязгаарлалтын систем нийцгүй буюу ШИЙДГҮЙ байна.
        int count_eyreg_elementuud = 0;
        int surug_row_idx = surug_entry.index;

        for (int j = 0; j < valid_cols; j++)
        {
          if (A[surug_row_idx][j].numerator > 0)
          {
            count_eyreg_elementuud++;
          }
        }

        // Тухайн сөрөг мөрөнд элементүүд бүгд эерэг байсан
        if (count_eyreg_elementuud == valid_cols - 1)
        {
          surug_entry.isAllPositive = true;
        }
      }
    }

    // small s.g logic is here
    int s = -1;
    int r = -1;
    
    // Сөрөг сул гишүүний мөр дотор бүр 
    // Боломжит сонгогдох сөрөг элэментүүд
    // Алийг нь сонгосноос хамаарч өөр, өөр 
    // Тулгуур шийд рүү хүргэх тул 
    // Тулгуур шийдийн огторгуйг хадгалав.
    vector<int> songoh;

    int min_surug_sg = INT_FAST16_MAX;

    for (auto & surug_entry: surug_sg_murnuud)
    {
      if (surug_entry.isAllPositive == false)
      {
        int surug_row_idx = surug_entry.index;

        for (int j = 0; j < valid_cols; j++)
        {
          if (A[surug_row_idx][j].numerator < 0)
          {
            // Гол багана сонгох
            s = j;
            break;
          }
        }
      }
    }

    // Симплекс харьцаанууд
    vector<tuple<int, Fraction>> simplex_ratios;

    
    for (size_t i = 0; i < valid_rows; i++)
    {
      if (sul_gishuud[i] == 0 )
      {
        // Бөхсөн Тулгуур шийд
      }
      else
      {
        double simplex_ratio = sul_gishuud[i] / A[i][s];

        simplex_ratios.push_back(make_tuple(i, simplex_ratio));
      }
    }

    

    if(a_rs.numerator != 0) 
    {
      cout << "Гол элемент: " << a_rs << endl;
      // cout << "Coeff array: " << free_vars[j] << endl;
      // cout << "original_vars array: " << original_vars[r] << endl;

      // Гол элемент (r, j) 
      jordan_step(A, rows, cols, r, c);

      cout << "  Хувиргалтын дараахь матриц: " << endl;
            
      printSystem(
          A, original_vars, free_vars,
          rows, cols
      );
    }
    else
    {
      cout << "Гол элемент 0 байна." << endl;
    }
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

  cout << "Зорилгын функцийн утгийг зайгаар оруулна уу, F= " << endl;

  vector<Fraction> F;
  getline(cin, line);
  stringstream ss(line);
  string token;

  while (ss >> token)
  {
    size_t slashPos = token.find('/');
    if (slashPos != string::npos)
    {
      int num = stoi(token.substr(0, slashPos));
      int den = stoi(token.substr(slashPos + 1));
      F.push_back(Fraction(num, den));
    }
    else
    {
      int num = stoi(token);
      F.push_back(Fraction(num, 1));
    }
  }
  
  int rows = fractionMatrix.size();
  cout << "Уншсан мөрийн тоо: " << rows 
        << " болон баганы тоо: " << cols << endl;

  // bool negative;
  // cout << "Матрицын элементүүдийг сөргөөр оруулсан уу? (1 - Тийм, 0 - Үгүй): " << endl;
  // int input;
  // cin >> input;
  // negative = (input != 0);
        

  // Чөлөөт үл мэдэгдэгчдийг унших
  vector<string> free_vars;

  string line;
  getline(cin, line);

  stringstream ss(line);
  string token;

  while (ss >> token)
  {
    free_vars.push_back(token);
  }

  cout << "Чөлөөт үл мэдэгдэгчид: " << endl;
  for (auto &val : free_vars) cout << val << " ";
  cout << endl;

  // Үндсэн үл мэдэгдэгчдийг унших

  vector<string> original_vars;

  string line;
  getline(cin, line);

  stringstream ss(line);
  string token;

  while (ss >> token)
  {
    original_vars.push_back(token);
  }

  cout << "Үндсэн үл мэдэгдэгчид : " << endl;
  for (auto &val : original_vars) cout << val << " ";
  cout << endl;

  // 2-р хувиргалтанд бэлдэж элементүүдийг сөрөг болгох

  Fraction **A = new Fraction*[rows];

  vector<tuple<int, Fraction>> sul_gishuud;

  for (int i = 0; i < rows; i++)
  {
      A[i] = new Fraction[cols];

      for (int j = 0; j < cols; j++)
      {
        // Сүүлийн мөр болон баганыг сөрөг тэмдэгтэй болгохгүй
        // Учир нь Зорилгын функц болон сул гишүүд оршиж буй
        if (j == cols - 1)
        {
          // Сул гишүүд
          A[i][j] = Fraction(
            fractionMatrix[i][j].numerator, 
            fractionMatrix[i][j].denominator);
          
          sul_gishuud.push_back(make_tuple(i, A[i][j]));
        }
        else if(i == rows - 1)
        {
          // Зорилгын функцын мөор
          A[i][j] = Fraction(
            fractionMatrix[i][j].numerator, 
            fractionMatrix[i][j].denominator);
        }
        else
        {
          A[i][j] = Fraction(
            -fractionMatrix[i][j].numerator, 
            fractionMatrix[i][j].denominator);
        }
      }
  }

  cout << "Сул гишүүд : " << endl;
  for (const auto &[index, val_frac] : sul_gishuud)
  {
    cout << index << "-р мөр, сул гишүүний утга: " << val_frac << endl;
  }
  
  cout << endl;

  cout << endl << "A матриц + Зорилгын функц: " << endl;

  printMatrix(A, rows, cols);

  // Гаусс-Жорданы 2-р хувиргалт
  tulguur_shiid(
      A, rows, cols, 
      free_vars,
      original_vars,
      sul_gishuud
  );

  // Санах ойгоо цэвэрлэх
  for(int i = 0; i < rows; i++)
  {
      delete[] A[i];
  }
  delete[] A;

  return 0;
}