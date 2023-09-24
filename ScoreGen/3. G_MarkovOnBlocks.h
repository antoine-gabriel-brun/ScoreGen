#pragma region head

	#pragma once

	using namespace System;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Drawing;
	using namespace System::Drawing::Drawing2D;

	#include "1. Generator.h"
	using namespace ScoreGenLib::Music;

#pragma endregion

namespace ScoreGenLib {
namespace Generators {

	// Générateur abstrait, qui suit une chaîne de Markov évolutive pour créer une séquence de blocks
	public ref class G_MarkovOnBlocks abstract : public Generator {

		// Types
		protected:
			// Instant, et matrice des probabilités à cet instant
			ref struct TimeAndMatrix {
			
				// Champs
				public:
					double            time;
					array<double, 2>^ matrix;

				// Constructeur
				public:
					TimeAndMatrix(double time, array<double, 2>^ matrix) :
						time  (time  ),
						matrix(matrix)
					{ }

			};
			// Générateur abstrait de blocks prenant en compte l'avancement
			ref class BlocksGeneratorForMarkov abstract {

				// Champs statiques
				private:
					static Type^ _thisClassType = ReflectionTools::getTypeByName_throwIfNotUnique("BlocksGeneratorForMarkov");

				// Accesseurs
				public:
					static property Type^ type {
						Type^ get() {
							return _thisClassType;
						}
					}

				// A implémenter
				public:
					virtual Block^ generate(double time01, int indexInGlobalSeq, int indexOfThisType) = 0;

			};

		// Champs
		private:
			int                                  _order                ;
			Markov^                              _markovChain          ;
			array<BlocksGeneratorForMarkov^, 1>^ _generators           ;
			array<int, 1>^                       _possibleInitialValues;
			int                                  _minDuration_1024th   ;
			int                                  _finalValue_or_m1     ;

		// Constructeur
		protected:
			G_MarkovOnBlocks() {
			
				// Initialisations, par appels aux fonctions implémentées dans les classes filles
				_order                 = getOrder();
				_markovChain           = gcnew Markov(_order, getProbas(), getSquareProbas());
				_generators            = (gcnew List<BlocksGeneratorForMarkov^>(getGenerators()))->ToArray(); // On n'utilise pas la fonction 'copy()' car celle-ci déclenche un bug tout à fait inexpliqué
				_possibleInitialValues = copy(getPossibleInitialValues());
				_minDuration_1024th    = getMinDuration_1024th();
				_finalValue_or_m1      = getFinalValue_or_m1();

				// Vérifier '_generators'
				{
					for each (BlocksGeneratorForMarkov^ generator in _generators) {
						if (generator == nullptr) {
							throw gcnew Exception("The generator classes shouldn't be null.");
						}
					}
				}
			
				// Vérifier '_possibleInitialValues'
				{
					if      (_possibleInitialValues == nullptr) throw gcnew ArgumentNullException;
					else if (_possibleInitialValues->Length == 0) throw gcnew Exception("There should be at least one possible initial value.");
					for each (int possibleInitialValue in _possibleInitialValues) {
						if (possibleInitialValue < 0 || possibleInitialValue >= _order) {
							throw gcnew Exception("The possible initial values should be in the range [0, order - 1].");
						}
					}
				}

				// Vérifier '_minDuration_1024th'
				if (_minDuration_1024th < 0) throw gcnew Exception("The duration shouldn't be negative.");

				// Vérifier '_finalValue_or_m1'
				if (_finalValue_or_m1 < -1 || _finalValue_or_m1 >= order) throw gcnew Exception("The final value should be in the range [0, order - 1].");

			}

		// Accesseurs
		public:
			property int order {
				int get() {
					return _order;
				}
			}

		// Implémentations
		public:
			virtual ScoreAndInfos^ generate() override {

				// Récupérer une liste de blocks, en faisant jouer la chaîne de Markov,
				{ /* en respectant '_minDuration_1024th' et '_finalValue_or_m1' */ }
				List<Block^>^ blocks = generateBlocks();

				// Déterminer le nombre de parties général (= maximum de celui de chaque block)
				int nbParts; {
					nbParts = 0;
					for each (Block^ block in blocks) {
						nbParts = Math::Max(nbParts, block->nbParts);
					}
				}

				// Créer le block
				Block^ block = gcnew Block(nbParts);

				// Ajouter tous les sous-blocks
				{
					int nextBar = 1;
					for each (Block^ sub in blocks) {
						int nbBarsInSub = sub->nbBars;
						int nbPartsInSub = sub->nbParts;
						for (int barInSub = 1; barInSub <= nbBarsInSub; barInSub++) {
							block->addBar(sub->timeSignature[barInSub]);
							for (int partInSub = 1; partInSub <= nbPartsInSub; partInSub++) {
								Group^ extractInSub = sub[partInSub, barInSub];
								block[partInSub, nextBar + barInSub - 1] = extractInSub;
							}
						}
						nextBar += nbBarsInSub;
					}
				}

				// Retourner
				return doOnGeneratedBlock(block);

			}

		// A implémenter
		protected:
			virtual int getOrder() = 0;
			virtual List<TimeAndMatrix^>^ getProbaStages() = 0;
			virtual array<BlocksGeneratorForMarkov^, 1>^ getGenerators() = 0;
			virtual array<int, 1>^ getPossibleInitialValues() = 0;
			virtual int getMinDuration_1024th() = 0;
			virtual int getFinalValue_or_m1() = 0;
			virtual bool getSquareProbas() = 0;
			virtual ScoreAndInfos^ doOnGeneratedBlock(Block^ block) = 0;

		// Fonctions
		private:
			array<TimeVariable^, 2>^ getProbas() {
			
				// Récupérer les probabilités au cours du temps (appel à la fonction 'getProbaStages()' de la classe fille)
				array<TimeAndMatrix^, 1>^ probaStages;
				int nbStages;
				{
					probaStages = getProbaStages()->ToArray();
					nbStages = probaStages->Length;
					if (nbStages < 2) throw gcnew Exception;
					for each (TimeAndMatrix^ timeAndMatrix in probaStages) {
						if (timeAndMatrix->matrix->GetUpperBound(0) != _order - 1) throw gcnew Exception;
						if (timeAndMatrix->matrix->GetUpperBound(1) != _order - 1) throw gcnew Exception;
					}
				}
			
				// Convertir en variables temporelles linéaires par morceaux
				array<TimeVariable^, 2>^ probas = gcnew array<TimeVariable^, 2>(_order, _order); {
					// Récupérer les moments
					array<double, 1>^ times = gcnew array<double, 1>(nbStages);
					for (int k = 0; k < nbStages; k++) times[k] = probaStages[k]->time;
					// Initialiser chaque case variable du tableau
					for (int i = 0; i < _order; i++) {
						for (int j = 0; j < _order; j++) {
							array<double, 1>^ valuesAt_ij = gcnew array<double, 1>(nbStages);
							for (int k = 0; k < nbStages; k++) valuesAt_ij[k] = probaStages[k]->matrix[i, j];
							probas[i, j] = gcnew TimeVariable_LinMulti(times, valuesAt_ij);
						}
					}
				}

				// Retourner
				return probas;

			}
			List<Block^>^ generateBlocks() {
			
				List<Block^>^ ans = gcnew List<Block^>;
		
				// (Ré)initialiser la chaîne de Markov
				_markovChain->initialize(_possibleInitialValues);
			
				// Générer en suivant l'ordre de la chaîne de Markov, en respectant '_minDuration_1024th' et '_finalValue_or_m1'
				int currentDuration_1024th = 0;
				int cpt = 0;
				array<int, 1>^ cptOfType = fill(0, _order);
				while (true) {
					// Calculer l'avancement
					double time01 = Math::Min(1., ((double)currentDuration_1024th) / (_minDuration_1024th - 1));
					// Récupérer l'élément suivant de la chaîne de Markov
					int index = _markovChain->next(time01);
					// Générer le block correspondant (et ajouter sa durée à 'currentDuration_1024th')
					Block^ newBlock = _generators[index]->generate(time01, cpt++, cptOfType[index]++);
					ans->Add(newBlock);
					currentDuration_1024th += newBlock->getDuration_1024th();
					// Décider de l'arrêt
					bool durationIsOver = currentDuration_1024th >= _minDuration_1024th;
					bool lastElementCanBeFinal = (_finalValue_or_m1 == -1) || (index == _finalValue_or_m1);
					if (durationIsOver && lastElementCanBeFinal) break;
				}

				// Retourner
				return ans;

			}

	};

}
}
