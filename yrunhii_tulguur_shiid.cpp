#include <iostream>
#include <vector>
#include <tuple>
#include "fraction.h"
#include <sstream>
#include <string>
#include <iomanip>
#include <unordered_map>
#include <algorithm>
#include <map>
using namespace std;

struct sul_gishuun_entry {
  int idx;
  Fraction value;
};

struct surug_entry {
  int idx;
  Fraction value;
  string status;
  bool hasNegElem;
};

struct candidate_entry {
  int row_idx;
  int col_idx;
};

struct simplex_ratio {
  int row_idx;
  Fraction value;
};

string fractionToString(const Fraction &f) {
    std::ostringstream oss;
    oss << f;
    return oss.str();
}

void printSulGishuudAll(
  vector<surug_entry> sul_gishuud
  )
{
  cout << "Сөрөг сул гишүүд: " << endl;

  for (const auto & entry : sul_gishuud)
  {
      cout << "Мөрийн дугаар : " << entry.idx + 1<< " "
          << "Утга: " << entry.value << " "
          << "Status: " << entry.status << " "
          << "Сөрөг элементтэй эсэх: " << entry.hasNegElem << endl;
  }
}

void printSulGishuud(
  vector<surug_entry> sul_gishuud
  )
{
  for (const auto & entry : sul_gishuud)
  {
      cout << "Мөрийн дугаар: " << entry.idx + 1 << " "
          << "Утга: " << entry.value << " "
          << "Сөрөг элементтэй эсэх: " << entry.hasNegElem << endl;
  }
}

void printSearchSpace(
  unordered_map<int, vector<int>> search_space
  )
{
  cout << "~~~Хайлтын огторгуй~~~ " << endl;

  if (!search_space.empty())
  {
      cout << "Сонгогдох Боломжит гол элементүүд: " << endl;
      for (const auto &pair: search_space) 
      {
        cout << pair.first + 1<< " мөрөнд -> ";
        for (int val : pair.second)
        {
          cout << val + 1 << "-р багана" << endl;
        }
        // cout << endl;
      }
  }
  else
  {
      cout << "Хоосон байна!" << endl;
  }
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

void remove_column(
  Fraction **&A,
  int rows,
  int &cols,
  int s
)
{
  Fraction** newA = new Fraction*[rows];

  for (int i = 0; i < rows; i++)
  {
    newA[i] = new Fraction[cols - 1];
  }

  for (int i = 0; i < rows; i++)
  {
    for (int j = 0, k = 0; j < cols; j++)
    {
      if (j != s)
      {
        newA[i][k++] = A[i][j];
      }
    }
  }

  
  for (size_t i = 0; i < (size_t)rows; i++)
  {
    delete[] A[i];

  }
  delete [] A;
  
  A = newA;
  
  cols--;
  printMatrix(newA, rows, cols);

  cout << "deleted" << endl;
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
    vector<sul_gishuun_entry> &sul_gishuud,
    string sul_gishuun_direction
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
  // bool shiidgui_eseh = false;

  bool simplex_husnegt = false;
  while (!simplex_husnegt)
  {
    int teg_undsen_elementuud = 0;
    vector<int> teg_undsen_muruud;

    cout << "orig: " << endl;
    for (auto & var : original_vars)
    {
      cout <<var << " ";
    }
    cout << endl;

    for (int i = 0; i < (int)original_vars.size(); i ++)
    {
      if (original_vars[i] == "0")
      {
        teg_undsen_elementuud++;
        teg_undsen_muruud.push_back(i);
      }
    }

    reverse(teg_undsen_muruud.begin(), teg_undsen_muruud.end());

    if (teg_undsen_elementuud > 0)
    {

      while (teg_undsen_elementuud != 0)
      {
        // cout << "(anh) mur bagana: " << rows << " " << cols << endl;

        int r = teg_undsen_muruud.back();
        teg_undsen_muruud.pop_back();

        cout << "teg undsen moruud: " <<endl;
        for (auto &teg: teg_undsen_muruud)
        {
          cout << teg << " ";
        }
        cout << endl;

        // Гаргасан элементийг устгах
        
        cout << "Teg mur: " << endl;
        cout << r << endl;
        Fraction min_col_elem = Fraction(__INT_MAX__, 1);
        

        int s = -1;
        for (int j = 0; j < valid_cols; j++)
        {
          if (A[r][j] != 0)
          {
            // s = j;
            // break;
            if (A[r][j] == 1 || A[r][j] == -1)
            {
              s = j;
              break;
            }
            else
            {
              if (min_col_elem > A[r][j])
              {
                min_col_elem = A[r][j];
                s = j;
              }
            }
          }
        }

        cout << "Gol element: " << A[r][s] << endl;

        // cout << "Teg solih bagana: " << endl;
        // cout << s << endl;
      
        jordan_step(A, rows, cols, r ,s);

        // delete teg bagana

        string temp_elem;

        temp_elem = free_vars[s];
        free_vars[s] = original_vars[r];
        original_vars[r] = temp_elem;
        
        // cout << "ustgah" << endl;
        remove_column(A, rows, cols, s);
        free_vars.erase(remove(free_vars.begin(), free_vars.end(), "0"), free_vars.end());
        valid_cols--;
        // original_vars.erase(remove(original_vars.begin(), original_vars.end(), "0"), original_vars.end());
        // cout << "ustgah2" << endl;
        
        cout << "Original vars: " << endl;
        for (auto &var: original_vars)
        {
          cout << var << " ";
        }

        for (int i = 0; i < valid_rows; i++)
        {
          for (int j = 0; j < cols; j++)
          {
            if (j == cols - 1)
            {
              sul_gishuud[i].value = A[i][j];
            }
          }
        }

        cout << "Хувиргалтын дараахь Сул гишүүд : " << endl;
        for (const auto &surug_entry : sul_gishuud)
        {
          cout << surug_entry.idx + 1 << "-р мөр, сул гишүүний утга: " << surug_entry.value << endl;
        }

        for (int i = 0; i < rows; i++)
        {
          for (int j = 0; j < cols; j++)
          {
            cout << A[i][j] << " ";
          }
          cout << endl;
        }

        // cout << "mur bagana: " << rows << " " << cols << endl;

        // printMatrix(A, rows, cols);

        // cout << "Frree vars: " << endl;
        // for (auto & var: free_vars)
        // {
        //   cout << var << " ";
        // }
        cout << endl;
        printSystem(A, original_vars, free_vars, rows, cols);

        teg_undsen_elementuud--;
      }
    }
    cout << "Teg element" << teg_undsen_elementuud << endl;
    if (teg_undsen_elementuud == 0)
    {
      simplex_husnegt = true;
      break;
    }
  }

  sul_gishuud.pop_back();

  // cout << "sul gishuud: " << endl;
  // for (auto &gishuun : sul_gishuud)
  // {
  //   cout << gishuun.value << " ";

  // }
  // cout << endl;

  if (simplex_husnegt)
  {
    // int count = 4;
    while (!tulguur_shiid_oldson)
    {
      // Сөрөг сул гишүүдийг хайх
      // Олдохгүй бол Тулгуур шийд олдсон гэсэн үг.
      // bool surug_sul_gishuun_oldson = false;
      unordered_map<int, vector<int>> search_space;
      vector<surug_entry> canditate_sul_gishuud;
      // Сөрөг сул гишүүн байгаа мөрийн дугааруудыг хадгалах

      // Сөрөг сул гишүүдийг nested dictionary буюу vector<surug_entry> бүтцээр
      // хадгалсан ба эхний элемент нь мөрийн дугаарыг болон удаахь нь
      // Тухайн мөрөнд харгалзах сул гишүүний утгыг хадгална
      // Дараагаар сонгогдох боломжтой утгууд хадгалагдана
      // Иймд харгалзах мөрийн дугаарыг хадгалахад хялбар болно.

      // if (sul_gishuud.size() == 1)
      // {
      //   sul_gishuun_entry single_elem = sul_gishuud.back();

      //   int count_surug_sul_gishuud = 0;
      //   if (single_elem.idx < rows)
      //   {
      //     // 0-с бага сөрөг сул гишүүнийг хайх
      //     if (single_elem.value.numerator < 0)
      //     {
      //       count_surug_sul_gishuud++;
      //       // surug_entry.isNeg = true; 

      //       int count_eyreg_elementuud = 0;
      //       int surug_row_idx = single_elem.idx;
            
      //       for (int j = 0; j < valid_cols; j++)
      //       {
      //         if (A[surug_row_idx][j].numerator > 0)
      //         {
      //           // Нийт эерэг элементүүдийн тоог олох
      //           count_eyreg_elementuud++;
      //         }
      //       }
      //     }
            
      //     if (count_surug_sul_gishuud == 0)
      //     {
      //       // Ямар ч сөрөг сул гишүүн олдоогүй тул тулгуур шийд олдсон
      //       cout << "Ямар ч сөрөг сул гишүүн олдоогүй тул тулгуур шийд олдсон!" << endl;
      //       tulguur_shiid_oldson = true;
      //       break;
      //     }
      //   }
      // }
      // else
      // {
      

      int count_surug_sul_gishuud = 0;
        for (auto &surug_entry: sul_gishuud)
        {
          cout << "Surug!" << endl;
          cout << surug_entry.value << endl;
          if (surug_entry.idx < rows)
          {
            // 0-с бага сөрөг сул гишүүнийг хайх
            if (surug_entry.value.numerator < 0)
            {
              count_surug_sul_gishuud++;
              // surug_entry.isNeg = true; 

              int count_eyreg_elementuud = 0;
              int surug_row_idx = surug_entry.idx;
              
              for (int j = 0; j < valid_cols; j++)
              {
                if (A[surug_row_idx][j].numerator >= 0)
                {
                  // Нийт эерэг элементүүдийн тоог олох
                  count_eyreg_elementuud++;
                }
                else if(A[surug_row_idx][j].numerator < 0)
                {
                  // Сөрөг сул гишүүний мөр дотор сөрөг элемент буй тул
                  // Хайлтын огторгуйд хадгалж байна.
                  search_space[surug_row_idx].push_back(j);
                }
              }
              
              if (count_eyreg_elementuud != valid_cols)
              {
                // Тухайн мөрөнд сөрөг элэлемнт байгаа
                canditate_sul_gishuud.push_back({surug_row_idx, surug_entry.value, "surug", true});
              }
            }
          }
          else if (surug_entry.value.numerator == 0)
          {
            // Хэрэв Бөхсөн буюу сул гишүүн нь 0 бол түүнийг
            // Сонгогдох боломжтой сөрөг сул гишүүний векторт
            // Хадгалахгүй байснаар нэмэлт тооцоолол хийх шаардлагагүй болох
            // Буюу 0-ээс өөр бага симплекс харьцаатай мөрийг гол мөр болгоно.
            cout << "Бөхсөн Тулгуур шийд олдов: " << surug_entry.idx 
                << " " << surug_entry.value << endl;
          }
        }

      if (count_surug_sul_gishuud == 0)
      {
        // tulguur_shiid_oldson = true;
        cout << "Тулгуур шийд олдсон!" << endl;
        break;
      }
      // }

      if (canditate_sul_gishuud.empty())
      {
        // shiidgui_eseh = true;
        cout << "Хязгаарлалтын систем нийцгүй!" << endl;
        break;
      }

      printSulGishuudAll(canditate_sul_gishuud);

      printSearchSpace(search_space);

      // small s.g logic is here
      int r = -1;
      int s = -1;
      
      Fraction min_surug_sul_gishuun = Fraction(__INT_MAX__, 1);

      int candidate_row = -1;

      if (sul_gishuun_direction == "dooroos_deesh")
      {
        if (!canditate_sul_gishuud.empty()) 
        {
          surug_entry last_el = canditate_sul_gishuud.back();
          candidate_row = last_el.idx;
        }
        else
        {
          cout << "Canditate sul gishuud hooson baina!" << endl;
        }
      }
      else if (sul_gishuun_direction == "deerees_doosh")
      {
        if (!canditate_sul_gishuud.empty()) 
        {
          surug_entry first_el = canditate_sul_gishuud.front();
          candidate_row = first_el.idx;
        }
        else
        {
          cout << "Canditate sul gishuud hooson baina!" << endl;
        }
      }
      else if (sul_gishuun_direction == "min")
      {
        // Хамгийн цөөхөн болон хялбар хувиргалт хийх
        for (auto &surug_entry: canditate_sul_gishuud)
        {
          if (surug_entry.hasNegElem)
          {
            if (min_surug_sul_gishuun > surug_entry.value)
            {
              min_surug_sul_gishuun = surug_entry.value;
              candidate_row = surug_entry.idx;
            }
          }
        }
      }
      else
      {
        cout << "Утгагүй сөрөг сул гишүүний мөр сонгох чиглэл байна!" << endl;
      }

      // Сөрөг сул гишүүний мөрнөөс сөрөг элемент сонгох
      for (auto &pair : search_space)
      {
        if (pair.first == candidate_row)
        {
          s = pair.second.front();
        }
      }

      // Симплекс харьцаа
      vector<simplex_ratio> simplex_ratios;

      Fraction min_simplex_ratio = Fraction(__INT_MAX__, 1);

      cout << "Симплекс харьцаанууд: " << endl;

      cout << "candidate row: " << candidate_row << endl;

      for (int i = 0; i < valid_rows; i++)
      { 
        Fraction simplex_ratio;
        if (A[i][s].numerator != 0)
        {
          simplex_ratio = sul_gishuud[i].value / A[i][s];

          if (simplex_ratio > Fraction(0, 1) 
            && simplex_ratio != Fraction(0, 1))
          {
            if (min_simplex_ratio > simplex_ratio)
            {
              min_simplex_ratio = simplex_ratio;
              r = i;
            }
          }
        }

        cout << simplex_ratio << " ";
      }

      cout << "Хамгийн бага симплекс харьцаа: " << endl;
      cout << min_simplex_ratio << endl;
      
      Fraction a_rs = A[r][s];

      if(a_rs.numerator != 0) 
      {
        // cout << "Гол элемент: " << a_rs << endl;
        // cout << "Coeff array: " << free_vars[j] << endl;
        // cout << "original_vars array: " << original_vars[r] << endl;

        string temp_elem;

        // r <= valid rows (rows - 1)
        // s <= valid rows (cols - 1)
        // Энэ нь сул гишүүний багана болон
        // Зорилгын функцын мөрийг оролцуулахгүй
        temp_elem = free_vars[s];
        free_vars[s] = original_vars[r];
        original_vars[r] = temp_elem;

        // 0 үндсэн үл мэдэгдэгч нар нь 1, 1 ээрээ
        // хувиргалт хийж буй харгалзах чөлөөт үл
        // мэдэгдэгчидтэй солигдох тул
        // хувиргалт бүрт 0-баганыг хасах хувиргалт хийж болно
        int teg_bagana = -1;
        for (int j = 0; j < (int)free_vars.size(); j++)
        {
          if (free_vars[j] == "0")
          {
            teg_bagana = j;
          }
        }

        if (teg_bagana != -1)
        {
          cout << "0 мөр хасах" << endl;
          remove_column(A, rows, cols, teg_bagana);
        }

        // 0-баганыг арилгах
        // for (int i = 0; i < valid_rows; i++)
        // {
        //   for (int j = 0; j < valid_cols; j++)
        //   {
        //     if (teg_bagana = j)
        //       delete A[i][j];
        //   }
        // }
        // Гол элемент (r, j)     
        jordan_step(A, rows, cols, r, s);

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

      for (int i = 0; i < valid_rows; i++)
      {
        for (int j = 0; j < cols; j++)
        {
          if (j == cols - 1)
          {
            sul_gishuud[i].value = A[i][j];
          }
        }
      }

      cout << "Хувиргалтын дараахь Сул гишүүд : " << endl;
      for (const auto &surug_entry : sul_gishuud)
      {
        cout << surug_entry.idx + 1 << "-р мөр, сул гишүүний утга: " << surug_entry.value << endl;
      }
    
    }
  }
}

int main() 
{
  cout << "Мөрийн элементүүдийг зайгаар тусгаарлан оруулна уу." << endl;
  cout << "Хоосон мөр оруулбал дууссан гэж үзнэ." << endl;
  
  vector<vector<Fraction> > fractionMatrix;
  vector<string> initial_relations;
  vector<string> final_relations;

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
      string relation;
      {
        // Мөрөнд оруулсан элементүүдийг унших
        stringstream ss(line);
        string token;
        
        if (token == "=" || token == "<=" || token == ">=")
        {
          relation = token;
          break;
        }

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
    initial_relations.push_back(relation);
    final_relations.push_back(relation);
  }

  cout << "Relations: " << endl;
  for (int i = 0; i < (int)initial_relations.size(); i++)
  {
    cout << initial_relations[i] << " ";
    cout << final_relations[i] << " ";
  }

  cout << "Зорилгын функцийн утгийг зайгаар оруулна уу, F= " << endl;

  vector<Fraction> F;

  if(getline(cin, line))
  {
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
  }
  
  int rows = fractionMatrix.size();
  cout << "Уншсан мөрийн тоо: " << rows 
        << " болон баганы тоо: " << cols << endl;

  // bool negative;
  // cout << "Матрицын элементүүдийг сөргөөр оруулсан уу? (1 - Тийм, 0 - Үгүй): " << endl;
  // int input;
  // cin >> input;
  // negative = (input != 0);
        

  /* fractionMatrix-г Хязгаарлалтын систем болгон
  уншиж түүнээс тэгшитгэл болон тэнцэтгэл бишийн
  системүүдийг хувиргах*/

  // Хувьсагч бүрийн давтамжийг олох энэ нь
  // тухайн системд ганц байна уу, үгүй юу гэдэгт хэрэгтэй

  map<int, int> var_freq;

  for (int j = 0; j < cols - 1; j++)
  {
    for (int i = 0; i < rows; i++)
    {
      if (fractionMatrix[i][j].numerator != 0)
      {
        var_freq[j]++;
      }
    }
  }

  cout << "Freq vars: " << endl;
  for (auto &val : var_freq)
  {
    cout << "x" << val.first + 1 << " : " << val.second;
    cout << endl;
  }
  // Чөлөөт үл мэдэгдэгчдийг унших
  cout << "Чөлөөт үл мэдэгдэгчдийг оруулна уу: " << endl;
  vector<string> free_vars;
  if(getline(cin, line)) 
  {
      stringstream ss(line);
      string token;
      while (ss >> token)
      {
          free_vars.push_back(token);
      }
  }

  // Үндсэн үл мэдэгдэгчдийг унших
  cout << "Үндсэн үл мэдэгдэгчдийг оруулна уу: " << endl;
  vector<string> original_vars;
  if(getline(cin, line)) 
  {
    stringstream ss(line);
    string token;
    while (ss >> token)
    {
        original_vars.push_back(token);
    }
  }

  free_vars.push_back("с.г");
  original_vars.push_back("F");

  cout << "Чөлөөт үл мэдэгдэгчид: " << endl;
  for (auto &val : free_vars) cout << val << " ";
  cout << endl;

  cout << "Үндсэн үл мэдэгдэгчид : " << endl;
  for (auto &val : original_vars) cout << val << " ";
  cout << endl;

  // 2-р хувиргалтанд бэлдэж элементүүдийг сөрөг болгох

  // Зорилгын функцыг нэмэх
  fractionMatrix.push_back(F);
  
  for (auto &val : fractionMatrix)
  {
    for (auto &elem : val)
    {
      cout << elem << " ";
    }
    cout << endl;
  }

  rows++;

  vector<sul_gishuun_entry> sul_gishuud;
  Fraction **A = new Fraction*[rows];

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
          
          sul_gishuud.push_back({i, A[i][j]});
        }
        // else if(i == rows - 1)
        // {
        //   // Зорилгын функцын мөор
        //   A[i][j] = Fraction(
        //     fractionMatrix[i][j].numerator, 
        //     fractionMatrix[i][j].denominator);
        // }
        else
        {
          A[i][j] = Fraction(
            -fractionMatrix[i][j].numerator, 
            fractionMatrix[i][j].denominator);
        }
      }
  }

  cout << "Сул гишүүд : " << endl;
  for (const auto &surug_entry : sul_gishuud)
  {
    cout << surug_entry.idx + 1 << "-р мөр, сул гишүүний утга: " << surug_entry.value << endl;
  }
  
  cout << endl;

  cout << endl << "A матриц + Зорилгын функц: " << endl;

  // printMatrix(A, rows, cols);

  string sul_gishuun_direction = "min";

  // Гаусс-Жорданы 2-р хувиргалт
  tulguur_shiid(
      A, rows, cols, 
      free_vars,
      original_vars,
      sul_gishuud,
      sul_gishuun_direction
  );

  // Санах ойгоо цэвэрлэх
  for(int i = 0; i < rows; i++)
  {
      delete[] A[i];
  }
  delete[] A;

  return 0;
}