#pragma region head

	#pragma once

	using namespace System;

	#include "1. TimeVariable.h"

#pragma endregion

namespace ScoreGenLib {
namespace MathForMusic {
	
	// Cha�ne de Markov, avec les poids variant au cours du temps
	public ref class Markov {
		
		// Champs
		private:
			// Variables fixes
			int                      _order;
			array<TimeVariable^, 2>^ _probas;
			bool                     _squareProbas;
		private:
			// Variables d'utilisation
			int _current;
			int _nbGeneratedElements;

		// Constructeur
		public:
			Markov(int order, array<TimeVariable^, 2>^ probas, bool squareProbas) {
			
				// Initialiser les champs et faire les v�rifications
				construct(order, probas, squareProbas);
		
			}
			Markov(int order, array<double, 2>^ constProbas) {
			
				// Convertir les probabilit�s 'double' en 'TimeVariable_Const'
				array<TimeVariable^, 2>^ probas = gcnew array<TimeVariable^, 2>(order, order);
				for (int i = 0; i < order; i++) {
					for (int j = 0; j < order; j++) {
						probas[i, j] = gcnew TimeVariable_Const(constProbas[i, j]);
					}
				}

				// Initialiser les champs et faire les v�rifications
				construct(order, probas, false);
		
			}
		private:
			void construct(int order, array<TimeVariable^, 2>^ probas, bool squareProbas) {
			
				// Initialisation des champs
				_order               = order       ;
				_probas              = copy(probas);
				_squareProbas        = squareProbas;
				_current             = -1          ;
				_nbGeneratedElements = 0           ;
			
				// V�rifications sur '_order'
				if (_order <= 0) throw gcnew Exception("The order should be at least 1.");

				// V�rifications sur '_probas'
				if (_probas == nullptr) throw gcnew ArgumentNullException;
				if (_probas->GetUpperBound(0) != _order - 1 || _probas->GetUpperBound(1) != _order - 1) throw gcnew Exception("The dimensions of the probabilities array should be (n * n), where 'n' is the order.");
				for each (TimeVariable^ proba in _probas) if (((Object^)proba) == nullptr) throw gcnew ArgumentNullException;

			}

		// Accesseurs
		public:
			property int order {
				int get() {
					return _order;
				}
			}
			property int nbGeneratedElements {
				int get() {
					return _nbGeneratedElements;
				}
			}

		// Utilisation de la cha�ne de Markov
		public:
			// (R�)initialisation
			void initialize(int index) {
				
				if (index < 0 || index >= _order) throw gcnew Exception("The initialisation index should be between 0 and (order - 1).");
			
				_current = index;
				_nbGeneratedElements = 0;

			}
			void initialize(array<int, 1>^ possibleIndices) {
				
				initialize(Rand::chooseIn(possibleIndices));

			}
			void initialize() {
				
				initialize(Rand::integer(0, _order));

			}
		public:
			// G�n�ration d'�l�ments
			int next(double time01) {
			
				// V�rifier que la cha�ne de Markov a d�j� �t� initialis�e
				if (_current < 0) throw gcnew Exception("To use the Markov chain, it should have been initialized using the function 'initialize()'.");

				// Est-ce le premier �l�ment ?
				if (_nbGeneratedElements == 0) {
					// C'est le premier �l�ment. Incr�menter le compteur d'�l�ments g�n�r�s
					_nbGeneratedElements++;
					// Renvoyer l'�l�ment sur lequel la cha�ne de Markov a �t� initialis�
					return _current;
				} else {
					// La cha�ne a d�j� g�n�r� un ou plusieurs �l�ments. R�cup�rer toutes les probabilit�s � l'instant pr�sent
					array<double, 1>^ probasNow;
					double sum;
					double max;
					{
						probasNow = gcnew array<double, 1>(_order);
						sum = 0;
						max = -1;
						for (int i = 0; i < _order; i++) {
							probasNow[i] = _probas[_current, i]->at(time01);
							if (probasNow[i] < 0) throw gcnew Exception;
							sum += probasNow[i];
							max = Math::Max(max, probasNow[i]);
						}
						// G�rer le cas o� toutes les probabilit�s sont nulles en d�cidant d'une probabilit� au hasard valant 1
						if (sum == 0) {
							probasNow[Rand::integer(0, _order)] = 1;
							sum = 1;
						}
					}
					// Si n�cessaire, normaliser la probabilit� maximale � 1 et tout �lever au carr�
					if (_squareProbas) {
						sum = 0;
						for (int i = 0; i < _order; i++) {
							probasNow[i] /= max;
							probasNow[i] *= probasNow[i];
							sum += probasNow[i];
						}
						max = 1;
					}
					// D�cider du nouvel indice courant
					int newCurrent; {
						newCurrent = -1;
						double val = sum * Rand::double01();
						double cpt = 0;
						for (int i = 0; i < _order; i++) {
							cpt += probasNow[i];
							if (cpt > val) {
								newCurrent = i;
								break;
							}
						}
						if (newCurrent == -1) newCurrent = Rand::integer(0, _order);
					}
					// Conserver le nouvel objet courant
					_current = newCurrent;
					// Incr�menter le compteur d'�l�ments g�n�r�s
					_nbGeneratedElements++;
					// Retourner
					return newCurrent;
				}

			}
			int next(double pos, int totalLength) {
				
				return next(pos / (totalLength - 1));

			}

	};

}
}
