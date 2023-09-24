#include "NEW_Test.hpp"

using namespace NEW_ScoreGenLib;
using namespace NEW_Music;

namespace {
	NEW_Exception exc = NEW_Exception("Echec du test.");
	// Chaque fichier de la bibliothèque a sa fonction de vérification
	void checkFile_Tools() {
		// PGCD et PPCM
		{
			if (gcd(10, 15) != 5) throw exc;
			if (gcd(123, 39) != 3) throw exc;
			if (gcd(16584, 7152) != 24) throw exc;
			if (lcm(16584, 7152) != 4942032) throw exc;
		}
		// Fraction
		{
			{
				bool badBehaviour;
				try {
					NEW_Fraction f_incorrect = NEW_Fraction(5, 0);
					badBehaviour = true; // si on est arrivé ici, c'est que (5, 0) n'a pas provoqué l'erreur voulue
				} catch (NEW_Exception exc) {
					badBehaviour = false;
				}
				if (badBehaviour) throw exc;
			}
			std::vector<NEW_Fraction> f = std::vector<NEW_Fraction>();
			f.push_back(NEW_Fraction(5, 3)       ); /* 5/3        */ if (f.back().get_a() !=   5 || f.back().get_b() != 3) throw exc;
			f.push_back(NEW_Fraction(7, 3)       ); /* 7/3        */ if (f.back().get_a() !=   7 || f.back().get_b() != 3) throw exc;
			f.push_back(NEW_Fraction(7, -4)      ); /* -7/4       */ if (f.back().get_a() !=  -7 || f.back().get_b() != 4) throw exc;
			f.push_back(NEW_Fraction(128, 12)    ); /* 32/3       */ if (f.back().get_a() !=  32 || f.back().get_b() != 3) throw exc;
			f.push_back(NEW_Fraction(-1280, -120)); /* 32/3 aussi */ if (f.back().get_a() !=  32 || f.back().get_b() != 3) throw exc;
			f.push_back(NEW_Fraction(0, -3)      ); /* 0          */ if (f.back().get_a() !=   0 || f.back().get_b() != 1) throw exc;
			f.push_back(NEW_Fraction(0, 3)       ); /* 0          */ if (f.back().get_a() !=   0 || f.back().get_b() != 1) throw exc;
			f.push_back(NEW_Fraction(0)          ); /* 0          */ if (f.back().get_a() !=   0 || f.back().get_b() != 1) throw exc;
			f.push_back(NEW_Fraction(8)          ); /* 8          */ if (f.back().get_a() !=   8 || f.back().get_b() != 1) throw exc;
			f.push_back(-11                      ); /* -11        */ if (f.back().get_a() != -11 || f.back().get_b() != 1) throw exc;
			f.push_back(0                        ); /* 0          */ if (f.back().get_a() !=   0 || f.back().get_b() != 1) throw exc;
			int nb = f.size();
			for (int i = 0; i < nb; i++) {
				NEW_Fraction frac1 = f[i];
				double val1 = frac1;
				if (frac1 != +frac1) throw exc;
				if (frac1 + (-frac1) != 0) throw exc;
				if (frac1 != -(-frac1)) throw exc;
				for (int j = 0; j < nb; j++) {
					NEW_Fraction frac2 = f[j];
					double val2 = frac2;
					if ((frac1 == frac2) ^ ((val1 <= val2 ? val2 - val1 : val1 - val2) < 0.0000001)) throw exc;
					if ((frac1 < frac2) ^ (val1 < val2)) throw exc;
					if ((frac1 > frac2) ^ (val1 > val2)) throw exc;
					if ((frac1 <= frac2) ^ (val1 <= val2)) throw exc;
					if ((frac1 >= frac2) ^ (val1 >= val2)) throw exc;
					double dist_sum = (double)(frac1 + frac2) - (((double)frac1) + ((double)frac2)); if (dist_sum < 0) dist_sum = -dist_sum;
					double dist_dif = (double)(frac1 - frac2) - (((double)frac1) - ((double)frac2)); if (dist_dif < 0) dist_dif = -dist_dif;
					double dist_mul = (double)(frac1 * frac2) - (((double)frac1) * ((double)frac2)); if (dist_mul < 0) dist_mul = -dist_mul;
					double dist_div = (frac2 == 0) ? 0 : (double)(frac1 / frac2) - (((double)frac1) / ((double)frac2)); if (dist_div < 0) dist_div = -dist_div;
					if (dist_sum > 0.0000001) throw exc;
					if (dist_dif > 0.0000001) throw exc;
					if (dist_mul > 0.0000001) throw exc;
					if (dist_div > 0.0000001) throw exc;
				}
			}
		}
		// Générateur aléatoire
		{
			// Le 'randomize' permet-il bien d'avoir plusieurs fois la même séquence ?
			{
				int nb = 100;
				std::vector<double> v1 = std::vector<double>(nb);
				std::vector<double> v2 = std::vector<double>(nb);
				NEW_Rand::randomize(1952); for (int i = 0; i < nb; i++) {
					v1[i] = (NEW_Rand::newDouble01() > 0.678) ? NEW_Rand::newDouble(20, 40) : (double)NEW_Rand::newInt(20, 40);
				}
				NEW_Rand::randomize(1952); for (int i = 0; i < nb; i++) {
					v2[i] = (NEW_Rand::newDouble01() > 0.678) ? NEW_Rand::newDouble(20, 40) : (double)NEW_Rand::newInt(20, 40);
				}
				for (int i = 0; i < nb; i++) {
					if (v1[i] != v2[i]) throw exc;
				}
			}
			// Les entiers couvrent-ils la bonne plage ?
			{
				NEW_Rand::randomize(19521952);
				int nb = 100000;
				int low = 92;
				int high = 328;
				int actualLow = 9999999;
				int actualHi = -9999999;
				for (int i = 0; i < nb; i++) {
					int val = NEW_Rand::newInt(low, high);
					if (val < actualLow) actualLow = val;
					if (val > actualHi) actualHi = val;
				}
				if (actualLow != low || actualHi != high) throw exc;
			}
			// Les doubles couvrent-ils la bonne plage ?
			{
				NEW_Rand::randomize(79879);
				int nb = 100000;
				double low = -328;
				double high = -92;
				double actualLow = 9999999.;
				double actualHi = -9999999.;
				for (int i = 0; i < nb; i++) {
					double val = NEW_Rand::newDouble(low, high);
					if (val < actualLow) actualLow = val;
					if (val > actualHi) actualHi = val;
				}
				if (actualLow < low || actualLow > low + 0.001 * (high - low)) throw exc;
				if (actualHi > high || actualHi < high - 0.001 * (high - low)) throw exc;
			}
			// Le choix dans les tableaux fonctionne-t-il ?
			{
				NEW_Rand::randomize(121212);
				std::vector<int> p = std::vector<int>();
				p.push_back(2); p.push_back(3); p.push_back(5); p.push_back(7); p.push_back(11); p.push_back(13); p.push_back(17); p.push_back(19);
				int lcm1 = 1, lcm2 = 1;
				for (int i = 0; i < 1000; i++) {
					lcm1 = lcm(lcm1, NEW_Rand::chooseIn(p));
					lcm2 = lcm(lcm2, NEW_Rand::chooseIn(p, 1, 5));
				}
				if (lcm1 != 2 * 3 * 5 * 7 * 11 * 13 * 17 * 19) throw exc;
				if (lcm2 != 3 * 5 * 7 * 11 * 13) throw exc;
			}
			// Le choix dans les listes fonctionne-t-il ?
			{
				NEW_Rand::randomize(78877887);
				std::list<int> l = std::list<int>();
				l.push_back(2); l.push_back(3); l.push_back(5); l.push_back(7); l.push_back(11); l.push_back(13); l.push_back(17); l.push_back(19);
				int lcm1 = 1, lcm2 = 1;
				for (int i = 0; i < 1000; i++) {
					lcm1 = lcm(lcm1, NEW_Rand::chooseIn(l));
					lcm2 = lcm(lcm2, NEW_Rand::chooseIn(l, 1, 5));
				}
				if (lcm1 != 2 * 3 * 5 * 7 * 11 * 13 * 17 * 19) throw exc;
				if (lcm2 != 3 * 5 * 7 * 11 * 13) throw exc;
			}
		}
		// New_LeftRight
		{
			// Essai 1
			{
				New_LeftRight<int> lr = New_LeftRight<int>(78, 81);
				if (lr.left != 78 || lr.right != 81) throw exc;
				lr.left = 445;
				lr.right = -112;
				if (lr.left != 445 || lr.right != -112) throw exc;
			}
			// Essai 2
			{
				// Créer un tableau
				std::vector<std::string> val = std::vector<std::string>(); val.push_back("a"); val.push_back(""); val.push_back("atrs"); val.push_back("b  gd"); val.push_back("  gd\r\n");
				// Diviser
				New_LeftRight<std::vector<std::string>>* leftRight; {
					std::vector<std::string> val_left = std::vector<std::string>(); val_left.push_back(val[0]); val_left.push_back(val[1]); val_left.push_back(val[2]);
					std::vector<std::string> val_right = std::vector<std::string>(); val_right.push_back(val[3]); val_right.push_back(val[4]);
					leftRight = new New_LeftRight<std::vector<std::string>>(val_left, val_right);
				}
				// Vérifier
				if (val[0] != leftRight->left [0]) throw exc;
				if (val[1] != leftRight->left [1]) throw exc;
				if (val[2] != leftRight->left [2]) throw exc;
				if (val[3] != leftRight->right[0]) throw exc;
				if (val[4] != leftRight->right[1]) throw exc;
				// Détruire
				delete leftRight;
			}
		}
	}
	void checkFile_Pitch() {
		// Alteration
		{
			if (NEW_Alteration::natural() != 0) throw exc;
			if (!((double)NEW_Alteration::doubleFlat() == -2 * (double)NEW_Alteration::sharp())) throw exc;
		}
		// Pitch
		{
			{
				if (!(NEW_Pitch('A', 1, 5) == NEW_Pitch('A', 1, 5))                    ) throw exc; // A == A
				if (!(NEW_Pitch('C', 0, 5) == NEW_Pitch('c', 0, 5))                    ) throw exc; // C == c
				if (!(NEW_Pitch('D', 1, 5) != NEW_Pitch('E', -1, 5))                   ) throw exc; // D# != Eb
				if (!(NEW_Pitch('C', 0, 4).get_midiValue() == 60)                      ) throw exc; // C4 == 60 (get)
				if (!(NEW_Pitch('B', 2, 4) == 73)                                      ) throw exc; // B##4 == 73 (operator)
				if (NEW_Pitch('D', 1, 5) != NEW_Pitch('E', -1, 5).get_midiValue()      ) throw exc; // D# == Eb en valeurs MIDI
				if (NEW_Pitch('F', 0.5, -1) != NEW_Pitch('G', -1.5, -1).get_midiValue()) throw exc; // F| == Gb. en valeurs MIDI
				if (NEW_Pitch(59.5) != NEW_Pitch('C', -0.5, 4).get_midiValue()         ) throw exc; // constructeur MIDI
			}
			{
				NEW_Pitch pp = NEW_Pitch('g', 1.5, 7);
				if (pp.get_midiValue () != 104.5) throw exc;
				if (pp.get_step      () != L'G' ) throw exc;
				if (pp.get_alteration() != 1.5  ) throw exc;
				if (pp.get_octave    () != 7    ) throw exc;
			}
			{
				NEW_Rand::randomize(678876);
				for (int i = 0; i < 10000; i++) {
					NEW_Pitch a = NEW_Pitch(NEW_Rand::newInt(0, 126) + (NEW_Rand::newChoice() ? 0 : 0.5));
					NEW_Pitch b = NEW_Pitch(NEW_Rand::newInt(0, 126) + (NEW_Rand::newChoice() ? 0 : 0.5));
					// Comparaison
					if ((a <  b                ) ^ (a.get_midiValue() <  b.get_midiValue())) throw exc;
					if ((a <  b.get_midiValue()) ^ (a.get_midiValue() <  b.get_midiValue())) throw exc;
					if ((a <= b                ) ^ (a.get_midiValue() <= b.get_midiValue())) throw exc;
					if ((a <= b.get_midiValue()) ^ (a.get_midiValue() <= b.get_midiValue())) throw exc;
					if ((a >  b                ) ^ (a.get_midiValue() >  b.get_midiValue())) throw exc;
					if ((a >  b.get_midiValue()) ^ (a.get_midiValue() >  b.get_midiValue())) throw exc;
					if ((a >= b                ) ^ (a.get_midiValue() >= b.get_midiValue())) throw exc;
					if ((a >= b.get_midiValue()) ^ (a.get_midiValue() >= b.get_midiValue())) throw exc;
					// Arithmétique
					if ((a + (b - a)) != b.get_midiValue()) throw exc;
					if ((a - a) != 0) throw exc;
					if (a.transpose(1.5) + 2.5 != a.transpose(-3).transpose(7).get_midiValue()) throw exc;
				}
			}
		}
	}
	void checkFile_Rhythms() {
		//
	}
}

void NEW_testNewLibrary() {
	checkFile_Tools();
	checkFile_Pitch();
	checkFile_Rhythms();
}
