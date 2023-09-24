#pragma region head

	#pragma once

	using namespace System;
	using namespace System::Collections;
	using namespace System::Collections::Generic;

	#include "1. Generator.h"
	using namespace ScoreGenLib::Music;
	using namespace ScoreGenLib::MathForMusic;

#pragma endregion

namespace ScoreGenLib {
namespace Generators {
namespace CloudMaker {

	ref class CloudMaker;

	// Positionnements possibles pour un motif
	public ref class PatternPossiblePosition {
		
		// Variables
		private:
			int           _val;
			RhythmFigure^ _rhythm;
		
		// Constructeur
		private:
			PatternPossiblePosition(int val, RhythmFigure^ rhythm) :
				_val   (val   ),
				_rhythm(rhythm)
			{ }
			
		// Accesseur
		public:
			property RhythmFigure^ rhythm {
				RhythmFigure^ get() {
					return _rhythm;
				}
			}

		// Valeurs
		public:
			static PatternPossiblePosition^ unspecified   = gcnew PatternPossiblePosition( 0, nullptr               );
			static PatternPossiblePosition^ onlyOnWhole   = gcnew PatternPossiblePosition( 1, RhythmFigure::_whole  );
			static PatternPossiblePosition^ onlyOnHalf    = gcnew PatternPossiblePosition( 2, RhythmFigure::_half   );
			static PatternPossiblePosition^ onlyOnQuarter = gcnew PatternPossiblePosition( 4, RhythmFigure::_quarter);
			static PatternPossiblePosition^ onlyOnEighth  = gcnew PatternPossiblePosition( 8, RhythmFigure::_eighth );
			static PatternPossiblePosition^ onlyOn16th    = gcnew PatternPossiblePosition(16, RhythmFigure::_16th   );
			static PatternPossiblePosition^ onAny32nd     = gcnew PatternPossiblePosition(32, RhythmFigure::_32nd   );
		
		// Op�rateurs
		public:
			static inline bool operator ==  (PatternPossiblePosition^ a, PatternPossiblePosition^ b) {
		
				return a->_val == b->_val;
		
			}
			static inline bool operator !=  (PatternPossiblePosition^ a, PatternPossiblePosition^ b) {
					
				return a->_val != b->_val;
		
			}
			static inline      operator int (PatternPossiblePosition^ a) {
		
				return a->_val;
		
			}
		
	};

	// Motif du nuage (entr�e et sortie du CloudMaker) --> d�river s'il faut empaqueter d'autres informations
	public ref class Pattern {
			
		// Champs
		private:
			array<Element^, 1>^      _elements;
			int                      _dur_1024th;
			PatternPossiblePosition^ _possiblePosition;

		// Constructeur
		public:
			Pattern(array<Element^, 1>^ elements, PatternPossiblePosition^ possiblePosition) :
				_elements        (copy(elements)  ),
				_possiblePosition(possiblePosition)
			{
				
				if ((Object^)_elements        == nullptr) throw gcnew ArgumentNullException;
				if ((Object^)possiblePosition == nullptr) throw gcnew ArgumentNullException;
					
				_dur_1024th = 0;
				for each (Element^ element in _elements) {
					_dur_1024th += element->duration_1024th;
				}

			}

		// Accesseurs
		public:
			property array<Element^, 1>^ elements {
				array<Element^, 1>^ get() {
					return copy(_elements);
				}
			}
			property int dur_1024th {
				int get() {
					return _dur_1024th;
				}
			}
			property PatternPossiblePosition^ possiblePosition {
				PatternPossiblePosition^ get() {
					return _possiblePosition;
				}
			}

	};

	// Motif avec sa probabilit� �volutive (entr�e)
	public ref class PatternWithProba {
			
		// Champs
		protected:
			Pattern^      _pattern;
			TimeVariable^ _proba;

		// Constructeurs
		public:
			PatternWithProba(Pattern^ pattern, TimeVariable^ proba) :
				_pattern(pattern),
				_proba  (proba  )
			{
				
				if ((Object^)_pattern == nullptr) throw gcnew ArgumentNullException;

			}
			PatternWithProba(array<Element^, 1>^ elements, PatternPossiblePosition^ possiblePosition, TimeVariable^ proba) :
				_pattern(gcnew Pattern(elements, possiblePosition)),
				_proba  (proba                                    )
			{ }

		// Accesseurs
		public:
			property Pattern^ pattern {
				Pattern^ get() {
					return _pattern;
				}
			}
			property TimeVariable^ proba {
				TimeVariable^ get() {
					return _proba;
				}
			}

		// Fonctions statiques
		public:
			static List<Pattern^>^ chooseRandomlyIn(array<PatternWithProba^, 1>^ patterns, double time01, int nbChoose) {
					
				int nbPatterns = patterns->Length;
				if (nbPatterns == 0) throw gcnew Exception("Couldn't choose one pattern in a void array.");

				// D�terminer la probabilit� de chaque pattern (affecter des probabilit�s d'autant plus grandes que la quantisation est importante)
				array<double, 1>^ probas = gcnew array<double, 1>(nbPatterns); {
					for (int i = 0; i < nbPatterns; i++) {
						PatternWithProba^ patternAndProba = patterns[i];
						probas[i] = patternAndProba->_proba->at(time01);
						probas[i] *= patternAndProba->_pattern->possiblePosition->rhythm->duration_1024th;
					}
				}

				// D�terminer la somme de toutes les probabilit�s
				double sum = 0; {
					for each (double proba in probas) {
						sum += proba;
					}
				}

				// Si la somme est nulle, toutes les probabilit�s sont nulles : il y a une erreur. [on pourrait aussi accepter de choisir un des patterns]
				if (sum == 0) throw gcnew Exception("Couldn't choose a pattern: the probability of all possible patterns is zero.");

				// Collecter 'nbChoose' patterns, choisis al�atoirement
				List<Pattern^>^ ans = gcnew List<Pattern^>; {
					for (int n = 0; n < nbChoose; n++) {
						// Choisir une valeur au hasard dans la plage 'sum'
						double randomVal = sum * Rand::double01();
						// Ajouter le pattern correspondant
						double current = 0;
						bool found = false;
						for (int i = 0; i < nbPatterns; i++) {
							current += probas[i];
							if (current >= randomVal) {
								ans->Add(patterns[i]->pattern);
								found = true;
								break;
							}
						}
						if (!found) ans->Add(patterns[nbPatterns - 1]->pattern);
					}
				}

				// Retourner
				return ans;

			}

	};

	// Motif plac� dans le temps (�l�ment du nuage de sortie)
	public ref class PatternWithPos {
			
		// Champs
		protected:
			Pattern^ _pattern;
			int      _pos_1024th;

		// Constructeurs
		public:
			PatternWithPos(Pattern^ pattern, int pos_1024th) :
				_pattern   (pattern   ),
				_pos_1024th(pos_1024th)
			{
				
				if ((Object^)_pattern == nullptr) throw gcnew ArgumentNullException;

			}

		// Accesseurs
		public:
			property Pattern^ pattern {
				Pattern^ get() {
					return _pattern;
				}
			}
			property int pos_1024th {
				int get() {
					return _pos_1024th;
				}
			}

	};

	// Objet permettant de cr�er un nuage complexe de densit� �volutive, et de le placer dans un block
	public ref class CloudMaker {

		// Champs
		private:
			List<PatternWithProba^>^ _patternsPool;
			int                     _length;
			RhythmFigure^           _baseRhythm;
			TimeVariable^           _density;

		// Constructeurs et fonctions pour la cr�ation du 'patterns pool'
		public:
			CloudMaker(int length, RhythmFigure^ baseRhythm, TimeVariable^ density) :
				_patternsPool(gcnew List<PatternWithProba^>),
				_length      (length                      ),
				_baseRhythm  (baseRhythm                  ),
				_density     (density                     )
			{
		
				if (_length <= 1) throw gcnew Exception("The length should be at least 2.");
				if ((Object^)baseRhythm == nullptr) throw gcnew ArgumentNullException;
		
			}
			void addPattern(PatternWithProba^ patternAndProba) {
			
				if ((Object^)patternAndProba == nullptr) throw gcnew ArgumentNullException;

				_patternsPool->Add(patternAndProba);

			}

		// Cr�er un nuage avec les 'patterns' demand�s
		public:
			array<PatternWithPos^, 1>^ createCloud() {
			
				// R�cup�rer la densit� souhait�e � chaque instant
				array<double, 1>^ requiredDensityAt = getDensitiesArray();

				// Placer � chaque instant de nombreux 'patterns', beaucoup plus que le nombre n�cessaire
				array<array<Pattern^, 1>^, 1>^ patternsAt = getLotsOfPatternsAsArray(requiredDensityAt);

				// Tableau indiquant quels patterns sont � conserver, en termes de probabilit�s (initialement, tous)
				array<array<double, 1>^, 1>^ keepPatternsAt = getInitialKeepPatternsArray(patternsAt);
			
				// Tableau indiquant � chaque position le nombre de patterns qui jouent simultan�ment (en tenant compte des probabilit�s)
				array<double, 1>^ actualDensityAt = calcActualDensities(patternsAt);

				// D�cimer les patterns conserv�s dans 'keepPatternsAt', en actualisant 'actualDensityAt', pour respecter 'requiredDensityAt'
				decimatePatterns(patternsAt, keepPatternsAt, actualDensityAt, requiredDensityAt);

				// Garder uniquement les patterns restants, et les mettre dans une collection (sans effort suppl�mentaire class�e par position)
				List<PatternWithPos^>^ ans = getRemainingPatternsSorted(patternsAt, keepPatternsAt);

				// Retourner
				return ans->ToArray();

			}
		private:
			array<double, 1>^ getDensitiesArray() {
		
				array<double, 1>^ ans = gcnew array<double, 1>(_length);

				for (int i = 0; i < _length; i++) {
					ans[i] = _density->at(i, _length);
				}

				return ans;

			}
			array<array<Pattern^, 1>^, 1>^ getLotsOfPatternsAsArray(array<double, 1>^ densityAt) {
			
				array<array<Pattern^, 1>^, 1>^ ans = gcnew array<array<Pattern^, 1>^, 1>(_length);
			
				// Il s'agit de placer � chaque instant de nombreux 'patterns', beaucoup plus que le nombre n�cessaire.
				for (int pos = 0; pos < _length; pos++) {
					List<Pattern^>^ patternsAtThisPosition = gcnew List<Pattern^>;
					// D�terminer le nombre (plus grand que n�cessaire) de 'patterns' qu'on souhaite ajouter � la position 'pos'
					int nb = 5 * (int)Math::Ceiling(densityAt[pos] + 0.01);
					// Trouver tous les patterns possibles � la position 'pos', avec leur probabilit�
					array<PatternWithProba^, 1>^ possiblePatterns = getAllPossiblePatternsAt(pos)->ToArray();
					if (possiblePatterns->Length > 0) {
						// Choisir 'nb' patterns au hasard en respectant les probabilit�s
						List<Pattern^>^ patternsToAdd = PatternWithProba::chooseRandomlyIn(possiblePatterns, ((double)pos) / (_length - 1), nb);
						// Les ajouter
						for each (Pattern^ pattern in patternsToAdd) patternsAtThisPosition->Add(pattern);
					}
					// Conserver sous forme de tableau
					ans[pos] = patternsAtThisPosition->ToArray();
				}

				// Retourner
				return ans;

			}
			List<PatternWithProba^>^ getAllPossiblePatternsAt(int pos) {
			
				List<PatternWithProba^>^ ans = gcnew List<PatternWithProba^>;

				// Parcourir tous les patterns dans '_patternsPool'
				for each (PatternWithProba^ pattern in _patternsPool) {
					// D�terminer la position du pattern, en 256�mes de noires
					int pos_1024th = pos * _baseRhythm->duration_1024th;
					// 1�re exigence : le pattern est-il bien cal� ? (certains patterns ne peuvent tomber que sur une noire, que sur une blanche, etc.)
					bool ok_possiblePosition; {
						int quantization_1024th = pattern->pattern->possiblePosition->rhythm->duration_1024th;
						ok_possiblePosition = (pos_1024th % quantization_1024th == 0);
					}
					// 2nde exigence : le pattern tient-il dans la dur�e totale ?
					bool ok_inRange; {
						int length_1024th = _length * _baseRhythm->duration_1024th;
						ok_inRange = pos_1024th + pattern->pattern->dur_1024th <= length_1024th;
					}
					// Ajouter le pattern s'il est correct
					if (ok_possiblePosition && ok_inRange) ans->Add(pattern);
				}

				// Retourner
				return ans;

			}
			array<double, 1>^ calcActualDensities(array<array<Pattern^, 1>^, 1>^ patternsAt) {
			
				array<double, 1>^ ans = gcnew array<double, 1>(_length);

				// Initialiser � z�ro
				for (int i = 0; i < _length; i++) ans[i] = 0;

				// Ajouter chaque pattern sur sa dur�e
				for (int pos = 0; pos < _length; pos++) {
					for each (Pattern^ pattern in patternsAt[pos]) {
						int quantize = _baseRhythm->duration_1024th;
						int patternLen = (pattern->dur_1024th + (quantize - 1)) / quantize;
						int end = pos + patternLen;
						for (int i = pos; i < end; i++) {
							ans[i] += 1;
						}
					}
				}

				// Retourner
				return ans;

			}
			array<array<double, 1>^, 1>^ getInitialKeepPatternsArray(array<array<Pattern^, 1>^, 1>^ patternsAt) {
			
				array<array<double, 1>^, 1>^ ans = gcnew array<array<double, 1>^, 1>(_length);

				for (int pos = 0; pos < _length; pos++) {
					int len = patternsAt[pos]->Length;
					ans[pos] = gcnew array<double, 1>(len);
					for (int i = 0; i < len; i++) {
						ans[pos][i] = 1.;
					}
				}

				return ans;

			}
			List<PatternWithPos^>^ getRemainingPatternsSorted(array<array<Pattern^, 1>^, 1>^ patternsAt, array<array<double, 1>^, 1>^ keepPatternsAt) {
			
				List<PatternWithPos^>^ ans = gcnew List<PatternWithPos^>;

				// Parcourir les positions
				for (int pos = 0; pos < _length; pos++) {
					int pos_1024th = pos * _baseRhythm->duration_1024th;
					int nbPatternsAtThisPos = patternsAt[pos]->Length;
					// Parcourir les patterns � cette position
					for (int i = 0; i < nbPatternsAtThisPos; i++) {
						// R�cup�rer le pattern et sa probabilit� d'�tre gard�
						Pattern^ pattern   = patternsAt    [pos][i];
						double   probaKeep = keepPatternsAt[pos][i];
						// D�cider si on garde le pattern (au hasard, et en suivant la probabilit� demand�e)
						bool keep;
						if      (probaKeep == 0                ) keep = false;
						else if (probaKeep == 1                ) keep = true;
						else if (probaKeep > 0 && probaKeep < 1) keep = Rand::double01() < probaKeep;
						else                                     throw gcnew Exception;
						// Si on garde le pattern, l'ajouter
						if (keep) ans->Add(gcnew PatternWithPos(pattern, pos_1024th));
					}
				}

				// Retourner
				return ans;

			}
		private:
			ref struct PatternIndex {
			
				public:
					int pos;
					int index;

				public:
					PatternIndex(int pos, int index) :
						pos  (pos  ),
						index(index)
					{ }

			};
			void decimatePatterns(array<array<Pattern^, 1>^, 1>^ patterns, array<array<double, 1>^, 1>^ keep_out, array<double, 1>^ actualDensity, array<double, 1>^ requiredDensity) {
			
				{ /* D�cimer les patterns dans 'keep_out', en actualisant 'actualDensity', pour respecter 'requiredDensity'. */ }

				// Cr�er un ordre de parcours al�atoire des patterns
				array<PatternIndex^, 1>^ path; {
					// R�cup�rer les coordonn�es de tous les patterns
					List<PatternIndex^>^ path_lst = gcnew List<PatternIndex^>;
					for (int pos = 0; pos < _length; pos++) {
						int nbPatternsAtThisPos = patterns[pos]->Length;
						for (int index = 0; index < nbPatternsAtThisPos; index++) {
							path_lst->Add(gcnew PatternIndex(pos, index));
						}
					}
					path = path_lst->ToArray();
					// M�langer
					scramble(path);
				}

				// Faire cinq passages d'ajustement des probabilit�s
				for (int n = 0; n < 5; n++) {
					adjustProbas_oneRound(patterns, keep_out, actualDensity, requiredDensity, path);
				}

			}
			void adjustProbas_oneRound(array<array<Pattern^, 1>^, 1>^ patterns, array<array<double, 1>^, 1>^ keep_out, array<double, 1>^ actualDensity, array<double, 1>^ requiredDensity, array<PatternIndex^, 1>^ path) {
			
				{ /* Ajuster les probas de 'keep_out' en parcourant 'path', en actualisant 'actualDensity', pour respecter 'requiredDensity'. */ }
			
				// Parcourir les patterns
				for each (PatternIndex^ patternIndex in path) {
					int pos   = patternIndex->pos  ;
					int index = patternIndex->index;
					Pattern^ pattern = patterns[pos][index];
					double oldProba  = keep_out[pos][index];
					// D�terminer o� commence et o� finit le pattern
					int patternBeg, patternEnd; {
						int quantization = _baseRhythm->duration_1024th;
						patternBeg = pos;
						patternEnd = pos + (pattern->dur_1024th + quantization - 1) / quantization;
					}
					// Calculer la nouvelle proba
					double probaIncr = getProbaIncrement(patternBeg, patternEnd, actualDensity, requiredDensity);
					double newProba = oldProba + probaIncr;
					if (newProba < 0) newProba = 0;
					if (newProba > 1) newProba = 1;
					// Enregistrer la nouvelle proba
					keep_out[pos][index] = newProba;
					// Actualiser 'actualDensity'
					{
						double adjustment = newProba - oldProba;
						for (int i = patternBeg; i < patternEnd; i++) {
							actualDensity[i] += adjustment;
						}
					}
				}

			}
			double getProbaIncrement(int patternBeg, int patternEnd, array<double, 1>^ actualDensity, array<double, 1>^ requiredDensity) {
			
				{ /* Incr�ment de probabilit� pour la zone patternBeg-patternEnd */ }

				// Calculer la densit� actuelle moyenne et la densit� souhait�e moyenne sur la plage sp�cifi�e
				double actualDensity_med = 0, requiredDensity_med = 0; {
					for (int i = patternBeg; i < patternEnd; i++) {
						actualDensity_med   += actualDensity  [i];
						requiredDensity_med += requiredDensity[i];
					}
					int nb = patternEnd - patternBeg;
					actualDensity_med   /= nb;
					requiredDensity_med /= nb;
				}

				// Retourner, sans se soucier que l'incr�ment fasse ou non rester la probabilit� dans la plage 0-1
				return requiredDensity_med - actualDensity_med;

			}

		// Convertir un nuage en block (avant d'appeler cette fonction, il est possible de modifier les PatternWithPos, par exemple les notes)
		public:
			static Block^ cloudToBlock(array<PatternWithPos^, 1>^ cloud) {
			
				// Classer les patterns par 'pos' croissante
				Array::Sort(cloud, gcnew CloudPatternWithPos_ComparerByPos);

				// Poser chaque 'pattern' dans une partie, en cr�ant autant de parties que n�cessaire
				array<List<PatternWithPos^>^, 1>^ distribution = distribute(cloud);

				// D�terminer le nombre de parties et cr�er le block
				int nbParts = distribution->Length;
				Block^ block = gcnew Block(nbParts);

				// Fixer une dur�e totale (instant de fin de l'�l�ment qui termine le plus tard arrondi), et cr�er une mesure unique de cette dur�e
				int duration_1024th = 256; {
					for each (List<PatternWithPos^>^ part in distribution) {
						for each (PatternWithPos^ patternAndPos in part) {
							duration_1024th = Math::Max(duration_1024th, patternAndPos->pos_1024th + patternAndPos->pattern->dur_1024th);
						}
					}
					while (duration_1024th % 256 != 0) duration_1024th++;
					block->addBar(qSignature((duration_1024th / 256) + "/4"));
				}

				// Ajouter les �l�ments dans chaque partie
				for (int part = 1; part <= nbParts; part++) {
					block[part, 1] = gcnew Group(getPartElements(distribution[part - 1], duration_1024th));
				}

				// Retourner
				return block;

			}
		private:
			ref class CloudPatternWithPos_ComparerByPos: public Collections::Generic::Comparer<PatternWithPos^> {
			
				public:
					virtual int Compare(PatternWithPos^ a, PatternWithPos^ b) override {
					
						int diff = a->pos_1024th - b->pos_1024th;

						if      (diff < 0) return -1;
						else if (diff > 0) return  1;
						else               return  0;

					}

			};
			static array<List<PatternWithPos^>^, 1>^ distribute(array<PatternWithPos^, 1>^ cloud) {

				// Nombre de parties que l'on a eu besoin de cr�er (au d�but : z�ro)
				int nbParts_now = 0;

				// Cr�er un tableau extensible contenant le moment atteint dans chaque partie (par convention, 4 �l�ments au d�but)
				array<int, 1>^ partsEnds_1024th = gcnew array<int, 1>(4); for (int i = 0; i < 4; i++) partsEnds_1024th[i] = 0;

				// Cr�er la structure de r�ponse, aussi comme un tableau extensible
				array<List<PatternWithPos^>^, 1>^ ans = gcnew array<List<PatternWithPos^>^, 1>(4);

				// Parcourir les �l�ments � placer, et les ajouter un par un � une partie libre (en cr�ant des parties d�s que n�cessaire)
				for each (PatternWithPos^ patternAndPos in cloud) {
					// R�cup�rer la position de l'�l�ment � placer
					int pos_1024th = patternAndPos->pos_1024th;
					// Trouver toutes les parties qui sont libres � cet instant
					array<int, 1>^ freeParts;
					int nbFreeParts;
					{
						List<int>^ freeParts_lst = gcnew List<int>;
						for (int i = 0; i < nbParts_now; i++) {
							if (pos_1024th >= partsEnds_1024th[i]) freeParts_lst->Add(i);
						}
						freeParts = freeParts_lst->ToArray();
						nbFreeParts = freeParts->Length;
					}
					// Si aucune, cr�er une nouvelle partie (et ajuster toutes les variables en ce sens)
					if (nbFreeParts == 0) {
						// Il y a maintenant une partie suppl�mentaire
						nbParts_now++;
						// Ajouter la nouvelle partie � 'partsEnds_1024th' (contenu : 0). Redimensionner si besoin.
						if (partsEnds_1024th->Length < nbParts_now) partsEnds_1024th = increaseSize(partsEnds_1024th, 2 * partsEnds_1024th->Length);
						partsEnds_1024th[nbParts_now - 1] = 0;
						// Ajouter la nouvelle partie � 'ans' (contenu : nouvelle liste vide). Redimensionner si besoin.
						if (ans->Length < nbParts_now) ans = increaseSize(ans, 2 * ans->Length);
						ans[nbParts_now - 1] = gcnew List<PatternWithPos^>;
						// Il y a maintenant une unique partie libre � l'instant demand� : la nouvelle partie
						freeParts = gcnew array<int, 1> { nbParts_now - 1 };
						nbFreeParts = 1;
					}
					// Choisir une partie libre au hasard
					int freePart = Rand::chooseIn(freeParts);
					// Ajouter l'�l�ment dans cette partie, et actualiser 'partsEnds_1024th'
					ans[freePart]->Add(patternAndPos);
					partsEnds_1024th[freePart] = patternAndPos->pos_1024th + patternAndPos->pattern->dur_1024th;
				}

				// Retourner, en r�duisant la taille du tableau extensible aux seules valeurs utiles
				return reduceSize(ans, nbParts_now);

			}
			static List<Element^>^ getPartElements(List<PatternWithPos^>^ patterns, int totalDuration_1024th) {
			
				List<Element^>^ ans = gcnew List<Element^>;

				// Ajouter chaque 'pattern', pr�c�d� du temps avant lui
				int currentTime_1024th = 0;
				for each (PatternWithPos^ patternAndPos in patterns) {
					// Ajouter la dur�e avant le 'pattern'
					array<Rest^, 1>^ rests = Rest::getSum(patternAndPos->pos_1024th - currentTime_1024th);
					for each (Rest^ rest in rests) ans->Add(rest);
					// Ajouter les �l�ments du 'pattern'
					for each (Element^ element in patternAndPos->pattern->elements) ans->Add(element);
					// Incr�menter le temps courant
					currentTime_1024th = patternAndPos->pos_1024th + patternAndPos->pattern->dur_1024th;
				}

				// Ajouter le temps restant sous forme de silences
				array<Rest^, 1>^ rests = Rest::getSum(totalDuration_1024th - currentTime_1024th);
				for each (Rest^ rest in rests) ans->Add(rest);

				// Retourner
				return ans;

			}
			static bool patternsAreSortedByPos(array<PatternWithPos^, 1>^ cloud) {
			
				int len = cloud->Length; if (len <= 1) return true;

				int currentPos = cloud[0]->pos_1024th;

				for (int i = 1; i < len; i++) {
					int newPos = cloud[i]->pos_1024th;
					if (newPos < currentPos) return false;
					currentPos = newPos;
				}

				return true;

			}

	};

}
}
}
