#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cstdlib>
#include <iomanip>

using namespace std;

// ------------------- Fraction Class Declaration -------------------

struct Fraction {
    int numerator;
    int denominator;

    // Constructor with default values
    Fraction(int num = 0, int den = 1);
    void normalize();

    // Overloaded operators
    Fraction operator+(const Fraction &other) const;
    Fraction operator-(const Fraction &other) const;
    Fraction operator*(const Fraction &other) const;
    Fraction operator/(const Fraction &other) const;

    bool operator==(const Fraction &other) const;
    bool operator!=(const Fraction &other) const;
    bool operator<(const Fraction &other) const;
    bool operator>(const Fraction &other) const;

    // Convert fraction to double
    double toDouble() const;

    // Friend operator for output
    friend ostream &operator<<(ostream &os, const Fraction &fr);
};

// ------------------- Fraction Class Implementation -------------------

// Helper function: Greatest Common Divisor
static int gcd(int a, int b) {
    a = (a < 0) ? -a : a;
    b = (b < 0) ? -b : b;
    return (b == 0) ? a : gcd(b, a % b);
}

Fraction::Fraction(int num, int den) : numerator(num), denominator(den) {
    if (den == 0) {
        throw runtime_error("Fraction: Denominator cannot be zero.");
    }
    normalize();
}

void Fraction::normalize() {
    int g = gcd(numerator, denominator);
    numerator   /= g;
    denominator /= g;
    if (denominator < 0) {
        denominator = -denominator;
        numerator   = -numerator;
    }
}

Fraction Fraction::operator+(const Fraction &other) const {
    int num = numerator * other.denominator + denominator * other.numerator;
    int den = denominator * other.denominator;
    return Fraction(num, den);
}

Fraction Fraction::operator-(const Fraction &other) const {
    int num = numerator * other.denominator - denominator * other.numerator;
    int den = denominator * other.denominator;
    return Fraction(num, den);
}

Fraction Fraction::operator*(const Fraction &other) const {
    int num = numerator * other.numerator;
    int den = denominator * other.denominator;
    return Fraction(num, den);
}

Fraction Fraction::operator/(const Fraction &other) const {
    if (other.numerator == 0) {
        throw runtime_error("Fraction: Division by zero fraction.");
    }
    int num = numerator * other.denominator;
    int den = denominator * other.numerator;
    return Fraction(num, den);
}

bool Fraction::operator==(const Fraction &other) const {
    return (numerator == other.numerator && denominator == other.denominator);
}

bool Fraction::operator!=(const Fraction &other) const {
    return !(*this == other);
}

double Fraction::toDouble() const {
    return static_cast<double>(numerator) / static_cast<double>(denominator);
}

bool Fraction::operator<(const Fraction &other) const {
    return numerator * other.denominator < other.numerator * denominator;
}

bool Fraction::operator>(const Fraction &other) const {
    return numerator * other.denominator > other.numerator * denominator;
}

ostream &operator<<(ostream &os, const Fraction &fr) {
    if (fr.numerator == 0) {
        os << 0;
    } else if (fr.denominator == 1) {
        os << fr.numerator;
    } else {
        os << fr.numerator << "/" << fr.denominator;
    }
    return os;
}

string fractionToString(const Fraction &f) {
  ostringstream oss;
  oss << f;
  return oss.str();
}

void printMatrix(Fraction **A, int rows, int cols) {
  for(int i = 0; i < rows; i++){
      for(int j = 0; j < cols; j++){
          cout << A[i][j] << "  ";
      }
      cout << endl;
  }
  cout << endl;
}

string center(const string &s, int width) {
  int len = s.size();
  if(len >= width) return s;
  int left = (width - len) / 2;
  int right = width - len - left;
  return string(left, ' ') + s + string(right, ' ');
}

void printSystem(Fraction **A, vector<string> &b_string, vector<string> &coeff_array, int rows, int cols) {
  const int COL_WIDTH = 6;
  cout << center("", COL_WIDTH) << " || ";
  for (auto &coeff: coeff_array)
      cout << center(coeff, COL_WIDTH) << " || ";
  cout << endl;
  
  for(int i = 0; i < rows; i++){
      cout << center(b_string[i], COL_WIDTH) << " || ";
      for(int j = 0; j < cols; j++){
          ostringstream oss;
          oss << A[i][j];
          cout << center(oss.str(), COL_WIDTH) << " || ";
      }
      cout << endl;
  }
  cout << endl;
}

void jordan_step(Fraction **A, int rows, int cols, int r, int s) {
  // Copy the current state of matrix A
  Fraction **A_init = new Fraction*[rows];
  for(int i = 0; i < rows; i++){
      A_init[i] = new Fraction[cols];
      for(int j = 0; j < cols; j++){
          A_init[i][j] = A[i][j];
      }
  }

  Fraction a_rs = A_init[r][s];
  cout << "Pivot element: (" << r + 1 << ", " << s + 1 << ") = " << a_rs << endl;

  Fraction one(1,1);
  A[r][s] = one / a_rs;

  for (int j = 0; j < cols; j++){
      if(j != s)
          A[r][j] = A[r][j] / a_rs;
  }

  Fraction minusOne(-1,1);
  for (int i = 0; i < rows; i++){
      if(i != r)
          A[i][s] = (A[i][s] * minusOne) / a_rs;
  }

  for(int i = 0; i < rows; i++){
      if(i != r){
          for(int j = 0; j < cols; j++){
              if(j != s){
                  Fraction term1 = A_init[i][j] * a_rs;
                  Fraction term2 = A_init[i][s] * A_init[r][j];
                  A[i][j] = (term1 - term2) / a_rs;
              }
          }
      }
  }

  for(int i = 0; i < rows; i++){
      delete[] A_init[i];
  }
  delete[] A_init;
}

void find_solutions(Fraction **A, int rows, int cols,
  vector<int> &chosen_r, vector<int> &chosen_c,
  const vector<Fraction> &b, vector<string> &solutions,
  bool is_chosen_and_is_consistent, bool is_consistent)
{
  if (is_consistent) {
      vector<Fraction> b_init = b;
      cout << "b_init:" << endl;
      for(auto &val : b_init) cout << val << " ";
      cout << endl;

      Fraction minusOne(-1,1);
      vector<Fraction> b_new(rows, minusOne);

      int chosen_count = (int)chosen_r.size();
      for(int i = 0; i < chosen_count; i++){
          b_new[ chosen_r[i] ] = b_init[ chosen_c[i] ];
      }

      cout << "Modified b:" << endl;
      for(auto &val : b_new) cout << val << " ";
      cout << endl << endl << "Calculating solutions..." << endl << endl;

      vector<int> free_cols;
      for (int j = 0; j < cols; j++){
          bool isChosen = false;
          for (int i = 0; i < chosen_count; i++){
              if (chosen_c[i] == j){
                  isChosen = true;
                  break;
              }
          }
          if (!isChosen)
              free_cols.push_back(j);
      }

      vector<Fraction> fraction_exact_solutions(rows, Fraction(0, 1));
      vector<vector<string>> parametric_solutions;

      if (!is_chosen_and_is_consistent) {
          for(int i = 0; i < (int)chosen_r.size(); i++){
              Fraction sumFrac(0, 1);
              for(int j = 0; j < (int)chosen_c.size(); j++){
                  sumFrac = sumFrac + (b_new[j] * A[i][j]);
              }
              fraction_exact_solutions[chosen_c[i]] = sumFrac;
              cout << "Row " << i + 1 << " sum = " << sumFrac << endl;
          }
          
          if (!free_cols.empty()){
              cout << "\nFree variables:" << endl;
              for (auto &val : free_cols)
                  cout << "x" << val + 1 << " ";
              cout << endl;
              for (int i = 0; i < (int)fraction_exact_solutions.size(); i++){
                  vector<string> rowExpr;
                  for (auto &j : free_cols){
                      int coeff_numerator = A[i][j].numerator;
                      int coeff_denominator = A[i][j].denominator;
                      rowExpr.push_back("(" + to_string(-coeff_numerator) + "/" +
                                         to_string(coeff_denominator) + ")" +
                                         " * x" + to_string(j + 1));
                  }
                  parametric_solutions.push_back(rowExpr);
              }
          }
      }
      else {
          for(auto &i : chosen_r){
              Fraction sumFrac(0, 1);
              for(auto &j : chosen_c){
                  sumFrac = sumFrac + (b_new[j] * A[i][j]);
              }
              fraction_exact_solutions[i] = sumFrac;
              cout << "Row " << i << " sum = " << sumFrac << endl;
          }
          cout << "\nFree variables:" << endl;
          for (auto &val : free_cols)
              cout << "x" << val + 1 << " ";
          cout << endl;
          for (int i = 0; i < (int)fraction_exact_solutions.size(); i++){
              vector<string> rowExpr;
              for (auto &j : free_cols){
                  int coeff_numerator = A[i][j].numerator;
                  int coeff_denominator = A[i][j].denominator;
                  rowExpr.push_back("(" + to_string(-coeff_numerator) + "/" +
                                     to_string(coeff_denominator) + ")" +
                                     " * x" + to_string(j + 1));
              }
              parametric_solutions.push_back(rowExpr);
          }
      }

      solutions.resize(rows);
      for (int i = 0; i < rows; i++){
          string fracStr = fractionToString(fraction_exact_solutions[i]);
          string paramExpr;
          if (i < (int)parametric_solutions.size()){
              for (size_t k = 0; k < parametric_solutions[i].size(); k++){
                  if(k > 0)
                      paramExpr += " + ";
                  paramExpr += parametric_solutions[i][k];
              }
          }
          solutions[i] = !paramExpr.empty() ? fracStr + " + " + paramExpr : fracStr;
      }
  }
}

void gauss_jordan_2nd(Fraction **A, int rows, int cols, const vector<Fraction> &b,
                      vector<string> &solutions, vector<string> &coeff_array,
                      bool is_consistent, vector<int> &chosen_c_pos) 
{
  vector<int> chosen_r;
  vector<int> chosen_c;
  bool is_chosen_and_is_consistent;
  int r = 0;
  vector<int> canditate_cols;
  vector<string> b_string;

  for (auto &elem : b) {
      ostringstream oss;
      oss << elem;
      b_string.push_back(oss.str());
  }

  for (int i = 0; i < cols; i++){
      canditate_cols.push_back(i);
  }

  printSystem(A, b_string, coeff_array, rows, cols);

  while(r < rows) {
      bool is_chosen = false;
      for(auto &j : canditate_cols){
          Fraction a_rs = A[r][j];
          if(a_rs.numerator != 0){
              bool is_used = false;
              for(size_t k = 0; k < chosen_r.size(); k++){
                  if(chosen_r[k] == r || chosen_c[k] == j){
                      is_used = true;
                      break;
                  }
              }
              if(!is_used){
                  chosen_r.push_back(r);
                  chosen_c.push_back(j);
                  
                  string temp_elem = coeff_array[j];
                  coeff_array[j] = b_string[r];
                  b_string[r] = temp_elem;
                  
                  jordan_step(A, rows, cols, r, j);
                  cout << "\nAfter transformation (row " << r + 1 << "):" << endl;
                  printSystem(A, b_string, coeff_array, rows, cols);
                  r++;
                  is_chosen = true;
                  break;
              }
          }
      }
      if(!is_chosen) {
          if (r < rows) {
              Fraction left_side = b[r];
              Fraction right_side = Fraction(0, 1);
              for (auto &col : chosen_c) {
                  right_side = right_side + A[r][col] * b[col];
              }
              if (left_side != right_side) {
                  cout << "Inconsistency at row " << r + 1 << ": "
                       << left_side << " != " << right_side << endl;
                  is_consistent = false;
              }
              else {
                  cout << left_side << " == " << right_side << endl
                       << "System is consistent." << endl;
                  cout << "Free variables: ";
                  for (auto &col : canditate_cols)
                      cout << col << " ";
                  cout << endl;
                  is_chosen_and_is_consistent = true;
              }
              break;
          }
      }
  }

  for (int i = 0; i < (int)chosen_c.size(); i++){
      chosen_c_pos.push_back(chosen_c[i]);
  }

  find_solutions(A, rows, cols, chosen_r, chosen_c, b, solutions, is_chosen_and_is_consistent, is_consistent);
}

int main() {
  cout << "Enter the matrix rows (elements separated by space; empty line to finish):" << endl;

  bool is_consistent = true;
  vector< vector<Fraction> > fractionMatrix;
  string line;
  int cols = -1;

  while (getline(cin, line)) {
      if (line.empty())
          break;
      vector<Fraction> row;
      stringstream ss(line);
      string token;
      while (ss >> token) {
          size_t slashPos = token.find('/');
          if (slashPos != string::npos) {
              int num = stoi(token.substr(0, slashPos));
              int den = stoi(token.substr(slashPos + 1));
              row.push_back(Fraction(num, den));
          }
          else {
              int num = stoi(token);
              row.push_back(Fraction(num, 1));
          }
      }
      if (cols < 0)
          cols = row.size();
      else if (row.size() != static_cast<size_t>(cols)) {
          cerr << "Inconsistent column count!" << endl;
          return 1;
      }
      fractionMatrix.push_back(row);
  }

  int rows = fractionMatrix.size();
  cout << "Read " << rows << " rows and " << cols << " columns." << endl;

  vector<string> coeff_array;
  for (int i = 0; i < cols; i++){
      coeff_array.push_back("x" + to_string(i + 1));
  }

  cout << "\nCoeff Array:" << endl;
  for (auto &val : coeff_array)
      cout << val << " ";
  cout << endl;

  Fraction **A = new Fraction*[rows];
  for (int i = 0; i < rows; i++){
      A[i] = new Fraction[cols];
      for (int j = 0; j < cols; j++){
          A[i][j] = Fraction(fractionMatrix[i][j].numerator, fractionMatrix[i][j].denominator);
      }
  }

  Fraction minusOne(-1,1);
  vector<Fraction> b(rows);
  cout << "\nEnter b vector (" << rows << " numbers):" << endl;
  for (int i = 0; i < rows; i++){
      int val;
      cin >> val;
      b[i] = Fraction(val, 1);
  }
  for (int i = 0; i < rows; i++){
      b[i] = b[i] * minusOne;
  }

  cout << "\nMatrix A:" << endl;
  printMatrix(A, rows, cols);

  cout << "Vector b:" << endl;
  for (auto &val : b)
      cout << val << " ";
  cout << endl << endl;

  vector<string> solutions;
  vector<int> chosen_c_pos;
  gauss_jordan_2nd(A, rows, cols, b, solutions, coeff_array, is_consistent, chosen_c_pos);

  cout << "\nSolutions:" << endl;
  for (size_t i = 0; i < solutions.size(); i++){
      cout << "x" << i + 1 << " = " << solutions[i] << endl;
  }
  cout << endl;

  for (int i = 0; i < rows; i++){
      delete[] A[i];
  }
  delete[] A;

  return 0;
}