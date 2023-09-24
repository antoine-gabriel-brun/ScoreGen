#ifndef NEW_TOOLS_HPP
#define NEW_TOOLS_HPP

#include <exception>
#include <string>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <vector>
#include <list>
#include <iterator>
#include <sstream>

namespace NEW_ScoreGenLib {
namespace NEW_Music {

	// Conversions vers std::string
	template<typename T> inline std::string toString(T val) { std::ostringstream stream; stream << val; return stream.str(); }

	// NEW_Exception : exception avec message intégré
	class NEW_Exception : public std::exception {
		// Champs
		private:
			char* _msg;
		// Constructeurs
		public:
			NEW_Exception();
			NEW_Exception(char* const msg);
		// Implémentations
		public:
			const char* what() const;
	};

	// PGCD et PPCM
	int gcd(int a, int b);
	int lcm(int a, int b);

	// Fraction non mutable, basée sur des entiers de type 'int', stockée sous forme unique :
	//   - Le dénominateur est stocké strictement positif : seul le numérateur porte le signe
	//   - La fraction est stockée simplifiée : pgcd(numérateur, dénominateur) = 1
	//   - La fraction nulle est toujours stockée comme 0 / 1
	class NEW_Fraction {
		// Champs
		private:
			int _a;
			int _b;
		// Constructeurs
		public:
			NEW_Fraction(int a, int b);
			NEW_Fraction(int a);
		private:
			NEW_Fraction();
		// Accesseurs
		public:
			inline int get_a() const { return _a; }
			inline int get_b() const { return _b; }
		// Opérateurs
		public:
			// Conversion
			inline operator double() const { return ((double)_a) / _b; }
			// Comparaison
			inline bool operator==(const NEW_Fraction& other) const { return _a == other._a && _b == other._b; }
			inline bool operator!=(const NEW_Fraction& other) const { return _a != other._a || _b != other._b; };
			inline bool operator> (const NEW_Fraction& other) const { return _a * other._b > _b * other._a; }
			inline bool operator< (const NEW_Fraction& other) const { return other > *this; }
			inline bool operator>=(const NEW_Fraction& other) const { return !(*this < other); }
			inline bool operator<=(const NEW_Fraction& other) const { return !(*this > other); }
			inline bool operator==(int other) const { return _a == other && _b == 1; }
			inline bool operator!=(int other) const { return _a != other || _b != 1; };
			inline bool operator> (int other) const { return _a > _b * other; }
			inline bool operator< (int other) const { return other > *this; }
			inline bool operator>=(int other) const { return !(*this < other); }
			inline bool operator<=(int other) const { return !(*this > other); }
			friend bool operator==(int f, const NEW_Fraction& g);
			friend bool operator!=(int f, const NEW_Fraction& g);
			friend bool operator> (int f, const NEW_Fraction& g);
			friend bool operator< (int f, const NEW_Fraction& g);
			friend bool operator>=(int f, const NEW_Fraction& g);
			friend bool operator<=(int f, const NEW_Fraction& g);
			// Arithmétique
			inline NEW_Fraction operator+() const { return *this; }
			NEW_Fraction operator-() const;
			friend NEW_Fraction operator+(const NEW_Fraction& f, const NEW_Fraction& g);
			friend NEW_Fraction operator-(const NEW_Fraction& f, const NEW_Fraction& g);
			friend NEW_Fraction operator*(const NEW_Fraction& f, const NEW_Fraction& g);
			friend NEW_Fraction operator/(const NEW_Fraction& f, const NEW_Fraction& g);
			friend NEW_Fraction operator+(const NEW_Fraction& f, int g);
			friend NEW_Fraction operator-(const NEW_Fraction& f, int g);
			friend NEW_Fraction operator*(const NEW_Fraction& f, int g);
			friend NEW_Fraction operator/(const NEW_Fraction& f, int g);
			friend NEW_Fraction operator+(int f, const NEW_Fraction& g);
			friend NEW_Fraction operator-(int f, const NEW_Fraction& g);
			friend NEW_Fraction operator*(int f, const NEW_Fraction& g);
			friend NEW_Fraction operator/(int f, const NEW_Fraction& g);
		// Fonctions
		public:
			inline bool isZero   () const { return _a == 0; }
			inline bool isInteger() const { return _b == 1; }
	};

	// Générateur aléatoire
	namespace NEW_Rand {
		// Initialisation optionnelle
		inline void randomize(unsigned int seed) { std::srand(seed); }
		inline void randomize() { std::srand((unsigned int)(((long int)std::time(nullptr)) % UINT_MAX)); }
		// Génération de nombres
		inline bool newChoice() { return std::rand() % 2 == 0; }
		inline int newInt(int high) { return std::rand() % (high + 1); }
		inline int newInt(int low, int high) { return low + newInt(high - low); }
		inline double newDouble01() { return ((double)rand()) / RAND_MAX; }
		inline double newDouble(double low, double high) { return low + newDouble01() * (high - low); }
		// Choix d'éléments
		template<typename T> inline T chooseIn(const std::vector<T>& elements) { return elements[newInt(elements.size() - 1)]; }
		template<typename T> inline T chooseIn(const std::vector<T>& elements, int posLow, int posHigh) { return elements[newInt(posLow, posHigh)]; }
		template<typename T> inline T chooseIn(const std::list<T>& elements) { auto iter = elements.begin(); std::advance(iter, newInt(elements.size() - 1)); return *iter; }
		template<typename T> inline T chooseIn(const std::list<T>& elements, int posLow, int posHigh) { auto iter = elements.begin(); std::advance(iter, newInt(posLow, posHigh)); return *iter; }
	}

	// Paire gauche-droite
	template<typename T> struct New_LeftRight {
		// Champs
		public:
			T left ;
			T right;
		// Constructeur
		public:
			New_LeftRight(const T& left, const T& right) :
				left (left ),
				right(right)
			{ }
	};

}
}

#endif
