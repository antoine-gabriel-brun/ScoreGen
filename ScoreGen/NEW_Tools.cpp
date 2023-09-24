#include "NEW_Tools.hpp"

namespace NEW_ScoreGenLib {
namespace NEW_Music {

	// NEW_Exception
	NEW_Exception::NEW_Exception() : _msg("") { }
	NEW_Exception::NEW_Exception(char* const msg) : _msg(msg) { }
	const char* NEW_Exception::what() const { return _msg; }

	// PGCD et PPCM (à valeurs positives)
	int gcd(int a, int b) {
		// Valeurs absolues
		if (a < 0) a = -a;
		if (b < 0) b = -b;
		// Algorithme d'Euclide
		int temp;
		while (b > 0) {
			temp = b;
			b = a % b;
			a = temp;
		}
		return a;
	}
	int lcm(int a, int b) {
		// PPCM en conservant le signe
		int ans = (a / gcd(a, b)) * b;
		// Retourner la valeur absolue
		return ans >= 0 ? ans : -ans;
	}

	// NEW_Fraction
	NEW_Fraction::NEW_Fraction(int a, int b) :
		_a(a),
		_b(b)
	{
		// Traiter le dénominateur nul (--> exception)
		if (_b == 0) throw NEW_Exception("The denominator shouldn't be zero.");
		// Traiter la fraction zéro
		if (_a == 0) _b = 1;
		// Traiter les fractions de dénominateur négatif
		if (_b < 0) {
			_a = -_a;
			_b = -_b;
		}
		// Simplifier par le PGCD
		int div = gcd(_a, _b);
		if (div != 1) {
			_a /= div;
			_b /= div;
		}
	}
	NEW_Fraction::NEW_Fraction(int a) :
		_a(a),
		_b(1)
	{ }
	NEW_Fraction::NEW_Fraction() { }
	inline bool operator==(int f, const NEW_Fraction& g) { return g == f; }
	inline bool operator!=(int f, const NEW_Fraction& g) { return g != f; }
	inline bool operator> (int f, const NEW_Fraction& g) { return f * g._b > g._a; }
	inline bool operator< (int f, const NEW_Fraction& g) { return g > f; }
	inline bool operator>=(int f, const NEW_Fraction& g) { return !(f < g); }
	inline bool operator<=(int f, const NEW_Fraction& g) { return !(f > g); }
	NEW_Fraction NEW_Fraction::operator-() const {
		NEW_Fraction ans = NEW_Fraction();
		ans._a = -_a;
		ans._b = _b;
		return ans;
	}
	NEW_Fraction operator+(const NEW_Fraction& f, const NEW_Fraction& g) {
		int new_b = lcm(f._b, g._b);
		int new_a = f._a * (new_b / f._b) + g._a * (new_b / g._b);
		return NEW_Fraction(new_a, new_b);
	}
	NEW_Fraction operator-(const NEW_Fraction& f, const NEW_Fraction& g) {
		int new_b = lcm(f._b, g._b);
		int new_a = f._a * (new_b / f._b) - g._a * (new_b / g._b);
		return NEW_Fraction(new_a, new_b);
	}
	NEW_Fraction operator*(const NEW_Fraction& f, const NEW_Fraction& g) {
		return NEW_Fraction(f._a * g._a, f._b * g._b);
	}
	NEW_Fraction operator/(const NEW_Fraction& f, const NEW_Fraction& g) {
		return NEW_Fraction(f._a * g._b, f._b * g._a);
	}
	NEW_Fraction operator+(const NEW_Fraction& f, int g) {
		NEW_Fraction ans = NEW_Fraction();
		ans._a = f._a + f._b * g;
		ans._b = f._b;
		return ans;
	}
	NEW_Fraction operator-(const NEW_Fraction& f, int g) {
		NEW_Fraction ans = NEW_Fraction();
		ans._a = f._a - f._b * g;
		ans._b = f._b;
		return ans;
	}
	NEW_Fraction operator*(const NEW_Fraction& f, int g) {
		return NEW_Fraction(f._a * g, f._b);
	}
	NEW_Fraction operator/(const NEW_Fraction& f, int g) {
		return NEW_Fraction(f._a, f._b * g);
	}
	NEW_Fraction operator+(int f, const NEW_Fraction& g) {
		return g + f;
	}
	NEW_Fraction operator-(int f, const NEW_Fraction& g) {
		NEW_Fraction ans = NEW_Fraction();
		ans._a = g._b * f - g._a;
		ans._b = g._b;
		return ans;
	}
	NEW_Fraction operator*(int f, const NEW_Fraction& g) {
		return g * f;
	}
	NEW_Fraction operator/(int f, const NEW_Fraction& g) {
		return NEW_Fraction(f * g._b, g._a);
	}

}
}
