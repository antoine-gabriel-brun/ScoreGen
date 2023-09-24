#pragma region head

	#pragma once

	using namespace System;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Drawing;
	using namespace System::Drawing::Drawing2D;

	#include "5. Signature.h"
	#include "3. Group.h"
	#include "4. For block.h"

#pragma endregion

namespace ScoreGenLib {
namespace Music {

	// Bloc manipulable de musique
	public ref class Block {

		// Types
		private:
			// Tableaux à 1 et 2 dimensions [partie ; mesure], indéxés à partir de 1, extensibles. 'Grid' ne communique que par copie profonde.
			template <class T> ref class Array {
		
				// Champs
				private:
					int          _nbBars  ;
					array<T, 1>^ _elements;

				// Constructeurs
				public:
					Array() :
						_nbBars(0),
						_elements(gcnew array<T, 1>(0))
					{ }
					Array(int nbBars) :
						_nbBars(nbBars)
					{
				
						if (nbBars < 0) throw gcnew Exception("There should be a positive number of bars." );
					
						_elements = gcnew array<T, 1>(nbBars);

					}
					Array(array<T, 1>^ elements) :
						_nbBars(elements->GetUpperBound(0) + 1),
						_elements(copy(elements))
					{
				
						for (int bar = 0; bar < _nbBars; bar++) _elements[bar] = elements[bar];
				
					}
					Array(Array<T>^ arrayToCopy) :
						_nbBars(arrayToCopy->_nbBars),
						_elements(copy(arrayToCopy->_elements))
					{ }
			
				// Accesseurs
				public:
					property int nbBars {
						int get() {
							return _nbBars;
						}
						void set(int new_nbBars) {
							if (new_nbBars < 0) {
								throw gcnew Exception("There should be a positive number of bars.");
							} else if (new_nbBars < _nbBars) {
								int removeNb = _nbBars - new_nbBars;
								removeBars(removeNb);
							} else if (new_nbBars > _nbBars) {
								int addNb = new_nbBars - _nbBars;
								addBars(addNb);
							}
						}
					}
					property T default[int] {
						T get(int bar_base1) {
							return _elements[bar_base1 - 1];
						}
						void set(int bar_base1, T value) {
							_elements[bar_base1 - 1] = value;
						}
					}

				// Ajout - insertion - suppression - extraction - copie de parties ou de mesures
				public:
					// Ajout
					void addBars(int nb) {
					
						// Vérifications
						if      (nb <  0) throw gcnew Exception("Impossible to add a negative number of bars.");
						else if (nb == 0) return;

						// Création du tableau redimensionné
						array<T, 1>^ new_elements = gcnew array<T, 1>(_nbBars + nb);

						// Remplissage du tableau redimensionné
						for (int bar = 0; bar < nbBars; bar++) {
							new_elements[bar] = _elements[bar];
						}

						// Actualisation des dimensions et du tableau
						_nbBars += nb;
						_elements = new_elements;

					}
					void addBar() {
					
						addBars(1);

					}
					// Insertion
					void insertBars(int indexFirstBarToAdd_base1, int nb) {
					
						// Vérifications
						if      (nb <  0) throw gcnew Exception("Impossible to insert a negative number of bars.");
						else if (nb == 0) return;
						else if (indexFirstBarToAdd_base1 == 0          ) throw gcnew Exception("Impossible to insert bars at index " + indexFirstBarToAdd_base1 + " : this index is 0 based.");
						else if (indexFirstBarToAdd_base1 <  0          ) throw gcnew Exception("Impossible to insert bars at a negative index.");
						else if (indexFirstBarToAdd_base1 >  _nbBars + 1) throw gcnew Exception("Impossible to insert bars at index " + indexFirstBarToAdd_base1 + " : there are only " + _nbBars + " bars.");

						// Argument en base 0
						int indexFirstBarToAdd = indexFirstBarToAdd_base1 - 1;

						// Création du tableau redimensionné
						array<T, 1>^ new_elements = gcnew array<T, 1>(_nbBars + nb);

						// Remplissage du tableau redimensionné
						for (int bar = 0; bar < indexFirstBarToAdd; bar++) {
							new_elements[bar] = _elements[bar];
						}
						for (int bar = indexFirstBarToAdd + nb; bar < _nbBars + nb; bar++) {
							new_elements[bar] = _elements[bar - nb];
						}

						// Actualisation des dimensions et du tableau
						_nbBars += nb;
						_elements = new_elements;

					}
					void insertBar(int indexBarToAdd_base1) {
					
						insertBars(indexBarToAdd_base1, 1);

					}
					// Suppression
					void removeBars(int indexFirstBarToRemove_base1, int nb) {
					
						// Vérifications
						if      (nb <  0) throw gcnew Exception("Impossible to remove a negative number of bars.");
						else if (nb == 0) return;
						else if (indexFirstBarToRemove_base1          == 0      ) throw gcnew Exception("Impossible to remove bars at index " + indexFirstBarToRemove_base1 + " : this index is 0 based.");
						else if (indexFirstBarToRemove_base1          <  0      ) throw gcnew Exception("Impossible to remove bars at a negative index.");
						else if (indexFirstBarToRemove_base1          >  _nbBars) throw gcnew Exception("Impossible to remove bars at index " + indexFirstBarToRemove_base1 + " : there are only " + _nbBars + " bars.");
						else if (indexFirstBarToRemove_base1 + nb - 1 >  _nbBars) throw gcnew Exception("Impossible to remove " + nb + " bars at index " + indexFirstBarToRemove_base1 + " : there are only " + _nbBars + " bars.");

						// Argument en base 0
						int indexFirstBarToRemove = indexFirstBarToRemove_base1 - 1;

						// Création du tableau redimensionné
						array<T, 1>^ new_elements = gcnew array<T, 1>(_nbBars - nb);

						// Remplissage du tableau redimensionné
						for (int bar = 0; bar < indexFirstBarToRemove; bar++) {
							new_elements[bar] = _elements[bar];
						}
						for (int bar = indexFirstBarToRemove + nb; bar < _nbBars; bar++) {
							new_elements[bar - nb] = _elements[bar];
						}

						// Actualisation des dimensions et du tableau
						_nbBars -= nb;
						_elements = new_elements;

					}
					void removeBar(int indexBarToRemove_base1) {
					
						removeBars(indexBarToRemove_base1, 1);

					}
					void removeBars(int nb) {
					
						removeBars(_nbBars - nb + 1, nb);

					}
					void removeBar() {
					
						removeBars(1);

					}
					// Extraction
					Array<T>^ extractBars(int indexFirstBarToExtract_base1, int nb) {

						// Vérifications
						if      (nb < 0) throw gcnew Exception("Impossible to extract a negative number of bars.");
						else if (indexFirstBarToExtract_base1          == 0      ) throw gcnew Exception("Impossible to extract bars at index " + indexFirstBarToExtract_base1 + " : this index is 0 based.");
						else if (indexFirstBarToExtract_base1          <  0      ) throw gcnew Exception("Impossible to extract bars at a negative index.");
						else if (indexFirstBarToExtract_base1          >  _nbBars) throw gcnew Exception("Impossible to extract bars at index " + indexFirstBarToExtract_base1 + " : there are only " + _nbBars + " bars.");
						else if (indexFirstBarToExtract_base1 + nb - 1 >  _nbBars) throw gcnew Exception("Impossible to extract " + nb + " bars at index " + indexFirstBarToExtract_base1 + " : there are only " + _nbBars + " bars.");

						// Variables du nouvel objet
						array<T, 1>^ new_elements = gcnew array<T, 1>(nb);

						// Variables de calcul
						int offset_bar = indexFirstBarToExtract_base1 - 1;
					
						// Remplissage du nouvel objet
						for (int bar = 0; bar < nb; bar++) {
							new_elements[bar] = _elements[bar + offset_bar];
						}

						// Retour
						return gcnew Array<T>(new_elements);

					}
					// Copie
					void copyBar(int indexBarSource_base1, int indexBarDestination_base1) {

						// Vérifications
						if      (indexBarDestination_base1 == indexBarSource_base1) return;
						else if (indexBarSource_base1      == 0      ) throw gcnew Exception("Impossible to copy from a bar at index " + indexBarSource_base1 + " : this index is 0 based.");
						else if (indexBarDestination_base1 == 0      ) throw gcnew Exception("Impossible to copy to a bar at index " + indexBarDestination_base1 + " : this index is 0 based.");
						else if (indexBarSource_base1      <  0      ) throw gcnew Exception("Impossible to copy from a bar at a negative index.");
						else if (indexBarDestination_base1 <  0      ) throw gcnew Exception("Impossible to copy to a bar at a negative index.");
						else if (indexBarSource_base1      >  _nbBars) throw gcnew Exception("Impossible to copy from a bar at index " + indexBarSource_base1 + " : there are only " + _nbBars + " bars.");
						else if (indexBarDestination_base1 >  _nbBars) throw gcnew Exception("Impossible to copy to bar a at index " + indexBarDestination_base1 + " : there are only " + _nbBars + " bars.");

						// Copie
						_elements[indexBarDestination_base1] = _elements[indexBarDestination_base1];
				
					}

			};
			template <class T> ref class Grid {
		
				// Champs
				private:
					int          _nbParts ;
					int          _nbBars  ;
					array<T, 2>^ _elements;

				// Constructeurs
				public:
					Grid() :
						_nbParts(0),
						_nbBars(0),
						_elements(gcnew array<T, 2>(0, 0))
					{ }
					Grid(int nbParts, int nbBars) :
						_nbParts(nbParts),
						_nbBars(nbBars)
					{
				
						if      (nbParts < 0) throw gcnew Exception("There should be a positive number of parts.");
						else if (nbBars  < 0) throw gcnew Exception("There should be a positive number of bars." );
					
						_elements = gcnew array<T, 2>(nbParts, nbBars);

					}
					Grid(array<T, 2>^ elements) :
						_nbParts(elements->GetUpperBound(0) + 1),
						_nbBars (elements->GetUpperBound(1) + 1),
						_elements(copy(elements))
					{ }
					Grid(Grid<T>^ gridToCopy) :
						_nbParts(gridToCopy->_nbParts),
						_nbBars(gridToCopy->_nbBars),
						_elements(copy(gridToCopy->_elements))
					{ }
			
				// Accesseurs
				public:
					property int nbParts {
						int get() {
							return _nbParts;
						}
						void set(int new_nbParts) {
							if (new_nbParts < 0) {
								throw gcnew Exception("There should be a positive number of parts.");
							} else if (new_nbParts < _nbParts) {
								int removeNb = _nbParts - new_nbParts;
								removeParts(removeNb);
							} else if (new_nbParts > _nbParts) {
								int addNb = new_nbParts - _nbParts;
								addParts(addNb);
							}
						}
					}
					property int nbBars {
						int get() {
							return _nbBars;
						}
						void set(int new_nbBars) {
							if (new_nbBars < 0) {
								throw gcnew Exception("There should be a positive number of bars.");
							} else if (new_nbBars < _nbBars) {
								int removeNb = _nbBars - new_nbBars;
								removeBars(removeNb);
							} else if (new_nbBars > _nbBars) {
								int addNb = new_nbBars - _nbBars;
								addBars(addNb);
							}
						}
					}
					property T default[int, int] {
						T get(int part_base1, int bar_base1) {
							return _elements[part_base1 - 1, bar_base1 - 1];
						}
						void set(int part_base1, int bar_base1, T value) {
							_elements[part_base1 - 1, bar_base1 - 1] = value;
						}
					}

				// Ajout - insertion - suppression - extraction - copie de parties ou de mesures
				public:
					// Ajout
					void addParts(int nb) {
					
						// Vérifications
						if      (nb <  0) throw gcnew Exception("Impossible to add a negative number of parts.");
						else if (nb == 0) return;

						// Création du tableau redimensionné
						array<T, 2>^ new_elements = gcnew array<T, 2>(_nbParts + nb, _nbBars);

						// Remplissage du tableau redimensionné
						for (int part = 0; part < nbParts; part++) {
							for (int bar = 0; bar < nbBars; bar++) {
								new_elements[part, bar] = _elements[part, bar];
							}
						}

						// Actualisation des dimensions et du tableau
						_nbParts += nb;
						_elements = new_elements;

					}
					void addPart() {
					
						addParts(1);

					}
					void addBars(int nb) {
					
						// Vérifications
						if      (nb <  0) throw gcnew Exception("Impossible to add a negative number of bars.");
						else if (nb == 0) return;

						// Création du tableau redimensionné
						array<T, 2>^ new_elements = gcnew array<T, 2>(_nbParts, _nbBars + nb);

						// Remplissage du tableau redimensionné
						for (int part = 0; part < nbParts; part++) {
							for (int bar = 0; bar < nbBars; bar++) {
								new_elements[part, bar] = _elements[part, bar];
							}
						}

						// Actualisation des dimensions et du tableau
						_nbBars += nb;
						_elements = new_elements;

					}
					void addBar() {
					
						addBars(1);

					}
					// Insertion
					void insertParts(int indexFirstPartToAdd_base1, int nb) {
					
						// Vérifications
						if      (nb <  0) throw gcnew Exception("Impossible to insert a negative number of parts.");
						else if (nb == 0) return;
						else if (indexFirstPartToAdd_base1 == 0           ) throw gcnew Exception("Impossible to insert parts at index " + indexFirstPartToAdd_base1 + " : this index is 0 based.");
						else if (indexFirstPartToAdd_base1 <  0           ) throw gcnew Exception("Impossible to insert parts at a negative index.");
						else if (indexFirstPartToAdd_base1 >  _nbParts + 1) throw gcnew Exception("Impossible to insert parts at index " + indexFirstPartToAdd_base1 + " : there are only " + _nbParts + " parts.");

						// Argument en base 0
						int indexFirstPartToAdd = indexFirstPartToAdd_base1 - 1;

						// Création du tableau redimensionné
						array<T, 2>^ new_elements = gcnew array<T, 2>(_nbParts + nb, _nbBars);

						// Remplissage du tableau redimensionné
						for (int part = 0; part < indexFirstPartToAdd; part++) {
							for (int bar = 0; bar < _nbBars; bar++) {
								new_elements[part, bar] = _elements[part, bar];
							}
						}
						for (int part = indexFirstPartToAdd + nb; part < _nbParts + nb; part++) {
							for (int bar = 0; bar < _nbBars; bar++) {
								new_elements[part, bar] = _elements[part - nb, bar];
							}
						}

						// Actualisation des dimensions et du tableau
						_nbParts += nb;
						_elements = new_elements;

					}
					void insertPart(int indexPartToAdd_base1) {
					
						insertParts(indexPartToAdd_base1, 1);

					}
					void insertBars(int indexFirstBarToAdd_base1, int nb) {
					
						// Vérifications
						if      (nb <  0) throw gcnew Exception("Impossible to insert a negative number of bars.");
						else if (nb == 0) return;
						else if (indexFirstBarToAdd_base1 == 0          ) throw gcnew Exception("Impossible to insert bars at index " + indexFirstBarToAdd_base1 + " : this index is 0 based.");
						else if (indexFirstBarToAdd_base1 <  0          ) throw gcnew Exception("Impossible to insert bars at a negative index.");
						else if (indexFirstBarToAdd_base1 >  _nbBars + 1) throw gcnew Exception("Impossible to insert bars at index " + indexFirstBarToAdd_base1 + " : there are only " + _nbBars + " bars.");

						// Argument en base 0
						int indexFirstBarToAdd = indexFirstBarToAdd_base1 - 1;

						// Création du tableau redimensionné
						array<T, 2>^ new_elements = gcnew array<T, 2>(_nbParts, _nbBars + nb);

						// Remplissage du tableau redimensionné
						for (int part = 0; part < _nbParts; part++) {
							for (int bar = 0; bar < indexFirstBarToAdd; bar++) {
								new_elements[part, bar] = _elements[part, bar];
							}
							for (int bar = indexFirstBarToAdd + nb; bar < _nbBars + nb; bar++) {
								new_elements[part, bar] = _elements[part, bar - nb];
							}
						}

						// Actualisation des dimensions et du tableau
						_nbBars += nb;
						_elements = new_elements;

					}
					void insertBar(int indexBarToAdd_base1) {
					
						insertBars(indexBarToAdd_base1, 1);

					}
					// Suppression
					void removeParts(int indexFirstPartToRemove_base1, int nb) {
					
						// Vérifications
						if      (nb <  0) throw gcnew Exception("Impossible to remove a negative number of parts.");
						else if (nb == 0) return;
						else if (indexFirstPartToRemove_base1          == 0       ) throw gcnew Exception("Impossible to remove parts at index " + indexFirstPartToRemove_base1 + " : this index is 0 based.");
						else if (indexFirstPartToRemove_base1          <  0       ) throw gcnew Exception("Impossible to remove parts at a negative index.");
						else if (indexFirstPartToRemove_base1          >  _nbParts) throw gcnew Exception("Impossible to remove parts at index " + indexFirstPartToRemove_base1 + " : there are only " + _nbParts + " parts.");
						else if (indexFirstPartToRemove_base1 + nb - 1 >  _nbParts) throw gcnew Exception("Impossible to remove " + nb + " parts at index " + indexFirstPartToRemove_base1 + " : there are only " + _nbParts + " parts.");

						// Argument en base 0
						int indexFirstPartToRemove = indexFirstPartToRemove_base1 - 1;

						// Création du tableau redimensionné
						array<T, 2>^ new_elements = gcnew array<T, 2>(_nbParts - nb, _nbBars);

						// Remplissage du tableau redimensionné
						for (int part = 0; part < indexFirstPartToRemove; part++) {
							for (int bar = 0; bar < _nbBars; bar++) {
								new_elements[part, bar] = _elements[part, bar];
							}
						}
						for (int part = indexFirstPartToRemove + nb; part < _nbParts; part++) {
							for (int bar = 0; bar < _nbBars; bar++) {
								new_elements[part - nb, bar] = _elements[part, bar];
							}
						}

						// Actualisation des dimensions et du tableau
						_nbParts -= nb;
						_elements = new_elements;

					}
					void removePart(int indexPartToRemove_base1) {
					
						removeParts(indexPartToRemove_base1, 1);

					}
					void removeParts(int nb) {
					
						removeParts(_nbParts - nb + 1, nb);

					}
					void removePart() {
					
						removeParts(1);

					}
					void removeBars(int indexFirstBarToRemove_base1, int nb) {
					
						// Vérifications
						if      (nb <  0) throw gcnew Exception("Impossible to remove a negative number of bars.");
						else if (nb == 0) return;
						else if (indexFirstBarToRemove_base1          == 0      ) throw gcnew Exception("Impossible to remove bars at index " + indexFirstBarToRemove_base1 + " : this index is 0 based.");
						else if (indexFirstBarToRemove_base1          <  0      ) throw gcnew Exception("Impossible to remove bars at a negative index.");
						else if (indexFirstBarToRemove_base1          >  _nbBars) throw gcnew Exception("Impossible to remove bars at index " + indexFirstBarToRemove_base1 + " : there are only " + _nbBars + " bars.");
						else if (indexFirstBarToRemove_base1 + nb - 1 >  _nbBars) throw gcnew Exception("Impossible to remove " + nb + " bars at index " + indexFirstBarToRemove_base1 + " : there are only " + _nbBars + " bars.");

						// Argument en base 0
						int indexFirstBarToRemove = indexFirstBarToRemove_base1 - 1;

						// Création du tableau redimensionné
						array<T, 2>^ new_elements = gcnew array<T, 2>(_nbParts, _nbBars - nb);

						// Remplissage du tableau redimensionné
						for (int part = 0; part < _nbParts; part++) {
							for (int bar = 0; bar < indexFirstBarToRemove; bar++) {
								new_elements[part, bar] = _elements[part, bar];
							}
							for (int bar = indexFirstBarToRemove + nb; bar < _nbBars; bar++) {
								new_elements[part, bar - nb] = _elements[part, bar];
							}
						}

						// Actualisation des dimensions et du tableau
						_nbBars -= nb;
						_elements = new_elements;

					}
					void removeBar(int indexBarToRemove_base1) {
					
						removeBars(indexBarToRemove_base1, 1);

					}
					void removeBars(int nb) {
					
						removeBars(_nbBars - nb + 1, nb);

					}
					void removeBar() {
					
						removeBars(1);

					}
					// Extraction
					Grid<T>^ extract(int indexFirstPartToExtract_base1, int nbPartsToExtract, int indexFirstBarToExtract_base1, int nbBarsToExtract) {

						// Vérifications
						if      (nbPartsToExtract < 0) throw gcnew Exception("Impossible to extract a negative number of parts.");
						else if (indexFirstPartToExtract_base1                        == 0       ) throw gcnew Exception("Impossible to extract parts at index " + indexFirstPartToExtract_base1 + " : this index is 0 based.");
						else if (indexFirstPartToExtract_base1                        <  0       ) throw gcnew Exception("Impossible to extract parts at a negative index.");
						else if (indexFirstPartToExtract_base1                        >  _nbParts) throw gcnew Exception("Impossible to extract parts at index " + indexFirstPartToExtract_base1 + " : there are only " + _nbParts + " parts.");
						else if (indexFirstPartToExtract_base1 + nbPartsToExtract - 1 >  _nbParts) throw gcnew Exception("Impossible to extract " + nbPartsToExtract + " parts at index " + indexFirstPartToExtract_base1 + " : there are only " + _nbParts + " parts.");
						if      (nbBarsToExtract < 0) throw gcnew Exception("Impossible to extract a negative number of bars.");
						else if (indexFirstBarToExtract_base1                       == 0      ) throw gcnew Exception("Impossible to extract bars at index " + indexFirstBarToExtract_base1 + " : this index is 0 based.");
						else if (indexFirstBarToExtract_base1                       <  0      ) throw gcnew Exception("Impossible to extract bars at a negative index.");
						else if (indexFirstBarToExtract_base1                       >  _nbBars) throw gcnew Exception("Impossible to extract bars at index " + indexFirstBarToExtract_base1 + " : there are only " + _nbBars + " bars.");
						else if (indexFirstBarToExtract_base1 + nbBarsToExtract - 1 >  _nbBars) throw gcnew Exception("Impossible to extract " + nbBarsToExtract + " bars at index " + indexFirstBarToExtract_base1 + " : there are only " + _nbBars + " bars.");

						// Variables du nouvel objet
						array<T, 2>^ new_elements = gcnew array<T, 2>(nbPartsToExtract, nbBarsToExtract);

						// Variables de calcul
						int offset_part = indexFirstPartToExtract_base1 - 1;
						int offset_bar  = indexFirstBarToExtract_base1  - 1;
					
						// Remplissage du nouvel objet
						for (int part = 0; part < nbPartsToExtract; part++) {
							for (int bar = 0; bar < nbBarsToExtract; bar++) {
								new_elements[part, bar] = _elements[part + offset_part, bar + offset_bar];
							}
						}

						// Retour
						return gcnew Grid<T>(new_elements);

					}
					Grid<T>^ extractParts(int indexFirstPartToExtract_base1, int nb) {
					
						return extract(indexFirstPartToExtract_base1, nb, 1, _nbBars);

					}
					Grid<T>^ extractBars(int indexFirstBarToExtract_base1, int nb) {
					
						return extract(1, _nbParts, indexFirstBarToExtract_base1, nb);

					}
					// Copie
					void copyPart(int indexPartSource_base1, int indexPartDestination_base1) {

						// Vérifications
						if      (indexPartDestination_base1 == indexPartSource_base1) return;
						else if (indexPartSource_base1      == 0       ) throw gcnew Exception("Impossible to copy from a part at index " + indexPartSource_base1 + " : this index is 0 based.");
						else if (indexPartDestination_base1 == 0       ) throw gcnew Exception("Impossible to copy to a part at index " + indexPartDestination_base1 + " : this index is 0 based.");
						else if (indexPartSource_base1      <  0       ) throw gcnew Exception("Impossible to copy from a part at a negative index.");
						else if (indexPartDestination_base1 <  0       ) throw gcnew Exception("Impossible to copy to a part at a negative index.");
						else if (indexPartSource_base1      >  _nbParts) throw gcnew Exception("Impossible to copy from a part at index " + indexPartSource_base1 + " : there are only " + _nbParts + " parts.");
						else if (indexPartDestination_base1 >  _nbParts) throw gcnew Exception("Impossible to copy to part a at index " + indexPartDestination_base1 + " : there are only " + _nbParts + " parts.");

						// Copie
						for (int bar = 0; bar < _nbBars; bar++) {
							_elements[indexPartDestination_base1, bar] = _elements[indexPartSource_base1, bar];
						}
				
					}
					void copyBar(int indexBarSource_base1, int indexBarDestination_base1) {

						// Vérifications
						if      (indexBarDestination_base1 == indexBarSource_base1) return;
						else if (indexBarSource_base1      == 0      ) throw gcnew Exception("Impossible to copy from a bar at index " + indexBarSource_base1 + " : this index is 0 based.");
						else if (indexBarDestination_base1 == 0      ) throw gcnew Exception("Impossible to copy to a bar at index " + indexBarDestination_base1 + " : this index is 0 based.");
						else if (indexBarSource_base1      <  0      ) throw gcnew Exception("Impossible to copy from a bar at a negative index.");
						else if (indexBarDestination_base1 <  0      ) throw gcnew Exception("Impossible to copy to a bar at a negative index.");
						else if (indexBarSource_base1      >  _nbBars) throw gcnew Exception("Impossible to copy from a bar at index " + indexBarSource_base1 + " : there are only " + _nbBars + " bars.");
						else if (indexBarDestination_base1 >  _nbBars) throw gcnew Exception("Impossible to copy to bar a at index " + indexBarDestination_base1 + " : there are only " + _nbBars + " bars.");

						// Copie
						for (int part = 0; part < _nbParts; part++) {
							_elements[part, indexBarDestination_base1] = _elements[part, indexBarDestination_base1];
						}
				
					}
		
			};

		// Champs
		private:
			int                _nbParts       ;
			int                _nbBars        ;
			Array<Signature^>^ _timeSignatures;
			Grid<Group^>^      _contents      ;

		// Constructeur
		public:
			Block() :
				_nbParts(0),
				_nbBars(0),
				_timeSignatures(gcnew Array<Signature^>),
				_contents(gcnew Grid<Group^>)
			{ }
			Block(int nbParts) :
				_nbParts(0),
				_nbBars(0),
				_timeSignatures(gcnew Array<Signature^>),
				_contents(gcnew Grid<Group^>)
			{
			
				addParts(nbParts);
			
			}
			Block(Block^ blockToCopy) :
				_nbParts(blockToCopy->_nbParts),
				_nbBars(blockToCopy->_nbBars),
				_timeSignatures(gcnew Array<Signature^>(blockToCopy->_timeSignatures)),
				_contents(gcnew Grid<Group^>(blockToCopy->_contents))
			{ }
		private:
			// Constructeur privé, sans vérifications et sans copie
			Block(int nbParts, int nbBars, Array<Signature^>^ timeSignatures, Grid<Group^>^ contents) :
				_nbParts(nbParts),
				_nbBars(nbBars),
				_timeSignatures(timeSignatures),
				_contents(contents)
			{ }

		// Accesseurs
		public:
			property int nbParts {
				int get() {
					return _nbParts;
				}
				void set(int new_nbParts) {
					_nbParts           = new_nbParts;
					_contents->nbParts = new_nbParts;
				}
			}
			property int nbBars {
				int get() {
					return _nbBars;
				}
				void set(int new_nbBars) {
					_nbBars                 = new_nbBars;
					_timeSignatures->nbBars = new_nbBars;
					_contents      ->nbBars = new_nbBars;
				}
			}
			property Signature^ timeSignature[int] {
				Signature^ get(int bar) {
					return _timeSignatures[bar];
				}
				void set(int bar, Signature^ value) {
					// Refuser de changer la signature si elle n'a pas la même durée que l'ancienne (sauf si la mesure est toute vide)
					{
						Signature^ current = _timeSignatures[bar];
						bool bad = current != nullptr && value->duration_1024th != current->duration_1024th;
						if (bad) {
							bool allVoid = true;
							for (int part = 1; part <= _nbParts; part++) {
								Group^ contents = _contents[part, bar];
								if (contents != nullptr && contents->nbElements > 0) {
									allVoid = false;
									break;
								}
							}
							if (allVoid) bad = false;
						}
						if (bad) throw gcnew Exception("Impossible to change the time signature because the new duration isn't the same as the current duration.");
					}
					// Changer la signature
					_timeSignatures[bar] = value;
				}
			}
			property Group^ default[int, int] {
				Group^ get(int part, int bar) {
					return _contents[part, bar];
				}
				void set(int part, int bar, Group^ contents) {
					// Traiter le cas d'un contenu vide
					if (((Object^)contents) == nullptr) {
						_contents[part, bar] = nullptr;
						return;
					}
					// Refuser de changer le contenu si la signature de la mesure n'est pas encore définie
					if (_timeSignatures[bar] == nullptr) throw gcnew Exception("Impossible to set the contents of the bar before its time signature is defined.");
					// Refuser de changer le contenu s'il n'a pas la bonne durée pour la signature de la mesure
					if (contents->duration_1024th != _timeSignatures[bar]->duration_1024th) throw gcnew Exception("Impossible to set the contents of the bar because the duration of the new contents doesn't match the time signature of the bar.");
					// Changer le contenu
					_contents[part, bar] = contents;
				}
			}

		// Conversion implicite en 'Score' et en 'ScoreAndInfos'
		public:
			operator Score^();
			operator ScoreAndInfos^();

		// Ajout - insertion - suppression - extraction - duplication de parties ou de mesures
		public:
			// Ajout
			void addParts(int nb) {
					
				_nbParts += nb;
				_contents->addParts(nb);

			}
			void addPart() {
					
				addParts(1);

			}
			void addBars(int nb, Signature^ timeSignature) {
			
				_nbBars += nb;
				_timeSignatures->addBars(nb);
				_contents->addBars(nb);

				for (int bar = _nbBars - nb + 1; bar <= _nbBars; bar++) {
					_timeSignatures[bar] = timeSignature;
				}

			}
			void addBars(int nb, array<Signature^, 1>^ timeSignatures) {
			
				if (nb != timeSignatures->Length) throw gcnew Exception("There should be 'nb' time signatures in 'timeSignatures'.");

				_nbBars += nb;
				_timeSignatures->addBars(nb);
				_contents->addBars(nb);

				int indexFirstBar = _nbBars - nb + 1;
				for (int bar = indexFirstBar; bar <= _nbBars; bar++) {
					_timeSignatures[bar] = timeSignatures[bar - indexFirstBar];
				}

			}
			void addBar(Signature^ timeSignature) {
					
				addBars(1, timeSignature);

			}
			void addBars(int nb, String^ timeSignature);
			void addBars(int nb, array<String^, 1>^ timeSignatures);
			void addBar(String^ timeSignature);
			// Insertion
			void insertParts(int indexFirstPartToAdd, int nb) {
					
				_nbParts += nb;
				_contents->insertParts(indexFirstPartToAdd, nb);

			}
			void insertPart(int indexPartToAdd) {
					
				insertParts(indexPartToAdd, 1);

			}
			void insertBars(int indexFirstBarToAdd, int nb, Signature^ timeSignature) {
					
				_nbParts += nb;
				_timeSignatures->insertBars(indexFirstBarToAdd, nb);
				_contents->insertBars(indexFirstBarToAdd, nb);

				for (int bar = indexFirstBarToAdd; bar < indexFirstBarToAdd + nb; bar++) {
					_timeSignatures[bar] = timeSignature;
				}

			}
			void insertBars(int indexFirstBarToAdd, int nb, array<Signature^, 1>^ timeSignatures) {
					
				_nbParts += nb;
				_timeSignatures->insertBars(indexFirstBarToAdd, nb);
				_contents->insertBars(indexFirstBarToAdd, nb);

				for (int bar = indexFirstBarToAdd; bar < indexFirstBarToAdd + nb; bar++) {
					_timeSignatures[bar] = timeSignatures[bar - indexFirstBarToAdd];
				}

			}
			void insertBar(int indexBarToAdd, Signature^ timeSignature) {
					
				insertBars(indexBarToAdd, 1, timeSignature);

			}
			// Suppression
			void removeParts(int indexFirstPartToRemove, int nb) {
					
				_nbParts -= nb;
				_contents->removeParts(indexFirstPartToRemove, nb);

			}
			void removePart(int indexPartToRemove) {
					
				removeParts(indexPartToRemove, 1);

			}
			void removeParts(int nb) {
					
				removeParts(_nbParts - nb + 1, nb);

			}
			void removePart() {
					
				removeParts(1);

			}
			void removeBars(int indexFirstBarToRemove, int nb) {
					
				_nbParts -= nb;
				_timeSignatures->removeBars(indexFirstBarToRemove, nb);
				_contents->removeBars(indexFirstBarToRemove, nb);

			}
			void removeBar(int indexBarToRemove) {
					
				removeBars(indexBarToRemove, 1);

			}
			void removeBars(int nb) {
					
				removeBars(_nbBars - nb + 1, nb);

			}
			void removeBar() {
					
				removeBars(1);

			}
			// Extraction
			Block^ extractSubblock(int indexFirstPartToExtract, int nbPartsToExtract, int indexFirstBarToExtract, int nbBarsToExtract) {
			
				int new_nbParts = nbPartsToExtract;
				int new_nbBars  = nbBarsToExtract;
				Array<Signature^>^   new_timeSignatures = _timeSignatures->extractBars(indexFirstBarToExtract, nbBarsToExtract);
				Grid<Group^>^ new_elements       = _contents      ->extract    (indexFirstPartToExtract, nbPartsToExtract, indexFirstBarToExtract, nbBarsToExtract);

				return gcnew Block(new_nbParts, new_nbBars, new_timeSignatures, new_elements);

			}
			Block^ extractParts(int indexFirstPartToExtract, int nb) {
					
				return extractSubblock(indexFirstPartToExtract, nb, 1, _nbBars);

			}
			Block^ extractPart(int indexPartToExtract) {
					
				return extractParts(indexPartToExtract, 1);

			}
			Block^ extractBars(int indexFirstBarToExtract, int nb) {
					
				return extractSubblock(1, _nbParts, indexFirstBarToExtract, nb);

			}
			Block^ extractBar(int indexBarToExtract) {
					
				return extractBars(indexBarToExtract, 1);

			}
			// Copie
			void copyPart(int indexPartSource, int indexPartDestination) {

				_contents->copyPart(indexPartSource, indexPartDestination);
				
			}
			void copyBar(int indexBarSource, int indexBarDestination) {

				_timeSignatures->copyBar(indexBarSource, indexBarDestination);
				_contents->copyBar      (indexBarSource, indexBarDestination);
				
			}

		// Ecriture de sous-blocs
		public:
			void write(int part, BarIndexAndTime^ time, Block^ blockToAdd, WriteOptions^ options) {
			
				// Dérouler toutes les options
				int bar = time->bar;
				int timeInBar_1024th = time->timeInBar_1024th;
				TimeSignaturesOverwriteBehaviour^ timeSignaturesOverwriteBehaviour = options->timeSignaturesOverwriteBehaviour;
				NotesOverwriteBehaviour^          notesOverwriteBehaviour          = options->notesOverwriteBehaviour         ;
				TooMuchPartsBehaviour^            tooMuchPartsBehaviour            = options->tooMuchPartsBehaviour           ;
				TooMuchBarsBehaviour^             tooMuchBarsBehaviour             = options->tooMuchBarsBehaviour            ;
					
				// Vérifications
				if      (part == 0           ) throw gcnew Exception("Impossible to write on a part at index " + part + " : this index is 0 based.");
				else if (part <  0           ) throw gcnew Exception("Impossible to write on a part at a negative index.");
				else if (part >  _nbParts + 1) throw gcnew Exception("Impossible to write on a part at index " + part + " : there are only " + _nbParts + " parts.");
				else if (bar  == 0           ) throw gcnew Exception("Impossible to write on a bar at index " + bar + " : this index is 0 based.");
				else if (bar  <  0           ) throw gcnew Exception("Impossible to write on a bar at a negative index.");
				else if (bar  >  _nbBars + 1 ) throw gcnew Exception("Impossible to write on a bar at index " + bar + " : there are only " + _nbBars + " bars.");
				
				// Selon 'timeSignaturesOverwriteBehaviour' : accepter - ou throw
				{
					if (timeSignaturesOverwriteBehaviour == TimeSignaturesOverwriteBehaviour::requestSameSignatures) {
						// Vérifier que les signatures rythmiques coïncident entre le bloc actuel et le bloc à ajouter
						if (!writeSubFct_timeSignaturesAreTheSame(bar, timeInBar_1024th, blockToAdd, this)) {
							throw gcnew Exception("Couldn't write with the option 'TimeSignaturesOverwriteBehaviour::requestSameSignatures' because time signatures are different.");
						}
					} else if (timeSignaturesOverwriteBehaviour == TimeSignaturesOverwriteBehaviour::adaptToCurrentSignatures) {
						// Rien à vérifier ici
					} else {
						// Valeur inconnue dans l'énumération
						throw gcnew Exception;
					}
				}

				// Selon 'tooMuchPartsBehaviour' : throw - ou réduire le bloc à ajouter - ou créer les parties manquantes
				{
					// Combien le bloc à insérer a-t-il de parties en trop pour être accueilli ?
					int tooMuch = Math::Max(0, part + blockToAdd->nbParts - 1 - _nbParts);
					// S'il y a des parties en trop, réagir selon 'tooMuchPartsBehaviour'
					if (tooMuch > 0) {
						if (tooMuchPartsBehaviour == TooMuchPartsBehaviour::requestEnoughParts) {
							// Exception
							throw gcnew Exception("Couldn't write with the option 'TooMuchPartsBehaviour::requestEnoughParts' because the current score hasn't enough parts.");
						} else if (tooMuchPartsBehaviour == TooMuchPartsBehaviour::addOnlyOnExistingParts) {
							// Réduire le bloc à ajouter
							int newNbPartsInBlock = blockToAdd->nbParts - tooMuch;
							if (newNbPartsInBlock <= 0) return;
							blockToAdd = blockToAdd->extractParts(1, newNbPartsInBlock);
						} else if (tooMuchPartsBehaviour == TooMuchPartsBehaviour::addLackingParts) {
							// Créer les parties manquantes
							addParts(tooMuch);
						} else {
							// Valeur inconnue dans l'énumération
							throw gcnew Exception;
						}
					}
				}

				// Selon 'tooMuchBarsBehaviour' : throw - ou réduire le bloc à ajouter - ou créer les mesures manquantes
				{
					// Combien le bloc à écrire a-t-il de temps en trop pour être accueilli ?
					int thisBlockDurationAfterInsertTimePoint_1024th = writeSubFct_getDurationAfterBarIndexAndTime_1024th(time);
					int insertBlockDuration_1024th = blockToAdd->getDuration_1024th();
					int tooMuch_1024th = Math::Max(0, insertBlockDuration_1024th - thisBlockDurationAfterInsertTimePoint_1024th);
					// S'il y a des parties en trop, réagir selon 'tooMuchPartsBehaviour'
					if (tooMuch_1024th > 0) {
						if (tooMuchBarsBehaviour == TooMuchBarsBehaviour::requestEnoughBars) {
							// Exception
							throw gcnew Exception("Couldn't write with the option 'TooMuchBarsBehaviour::requestEnoughBars' because the current score hasn't enough bars.");
						} else if (tooMuchBarsBehaviour == TooMuchBarsBehaviour::addOnlyOnExistingBars) {
							// Réduire le bloc à ajouter
							int newBlockDuration_1024th = thisBlockDurationAfterInsertTimePoint_1024th;
							if (newBlockDuration_1024th <= 0) return;
							blockToAdd = writeSubFct_getBlockWithReducedLength(blockToAdd, newBlockDuration_1024th);
						} else if (tooMuchBarsBehaviour == TooMuchBarsBehaviour::addOneBigBarAtTheEnd) {
							// Créer les mesures manquantes (une grande à la fin)
							Signature^ lastTimeSignature = _nbBars > 0 ? _timeSignatures[_nbBars] : nullptr;
							addBar(gcnew Signature(tooMuch_1024th, lastTimeSignature));
						} else if (tooMuchBarsBehaviour == TooMuchBarsBehaviour::addCommonBarsAtTheEnd) {
							// Créer les mesures manquantes (mesures en C, ou de même durée, à la fin)
							int newBarsDuration_1024th = 1024;
							int nbBars = (tooMuch_1024th + newBarsDuration_1024th - 1) / newBarsDuration_1024th;
							Signature^ lastTimeSignature = _nbBars > 0 ? _timeSignatures[_nbBars] : nullptr;
							bool lastHasGoodTimeSignatureDuration = (lastTimeSignature != nullptr && lastTimeSignature->duration_1024th == newBarsDuration_1024th);
							Signature^ newTimeSignature = lastHasGoodTimeSignatureDuration ? lastTimeSignature : Signature::common;
							addBars(nbBars, newTimeSignature);
						} else if (tooMuchBarsBehaviour == TooMuchBarsBehaviour::addBarsAtTheEndWithLastTimeSignature) {
							// Créer les mesures manquantes (mesures à la fin, de même signature que la dernière mesure actuelle)
							Signature^ lastBarTimeSignature = (_nbBars > 0) ? _timeSignatures[_nbBars] : Signature::common;
							int newBarsDuration_1024th = lastBarTimeSignature->duration_1024th;
							int nbBars = (tooMuch_1024th + newBarsDuration_1024th - 1) / newBarsDuration_1024th;
							addBars(nbBars, lastBarTimeSignature);
						} else {
							// Valeur inconnue dans l'énumération
							throw gcnew Exception;
						}
					}
				}

				// En suivant 'notesOverwriteBehaviour' : écrire dans chaque partie
				for (int part_source = 1; part_source <= blockToAdd->_nbParts; part_source++) {
					int part_destination = part + part_source - 1;
					// Ecrire la partie 'part_source' de 'block' dans la partie 'part_destination' du bloc actuel,
					{ /* à partir de (bar ; timeInBar_1024th) et jusqu'à la fin du bloc à ajouter */ }
					writeSubFct_writePart(blockToAdd, part_source, part_destination, bar, timeInBar_1024th, notesOverwriteBehaviour);
				}

			}
			void write(int part, BarIndexAndTime^ time, Block^ block) {
			
				write(part, time, block, gcnew WriteOptions());

			}
			void write(int part, int bar, Block^ block, WriteOptions^ options) {
				
				write(part, gcnew BarIndexAndTime(bar), block, options);

			}
			void write(int part, int bar, Block^ block) {
			
				write(part, gcnew BarIndexAndTime(bar), block);

			}
		private:
			static bool writeSubFct_timeSignaturesAreTheSame(int bar, int timeInBar_1024th, Block^ block_source, Block^ block_destination) {
				
				if (block_source->_nbBars == 0) return true;

				// La mesure 'bar' de 'block_destination' doit avoir après 'timeInBar_1024th' la durée de la 1ère mesure de 'block_source'
				int dur_remainingInDestination = block_destination->timeSignature[bar]->duration_1024th - timeInBar_1024th;
				int dur_firstBarInSource = block_source->timeSignature[0]->duration_1024th;
				if (dur_remainingInDestination != dur_firstBarInSource) return false;

				// Les mesures suivantes doivent être de signatures rythmiques égales en durée
				for (int barInSource = 2; barInSource <= block_source->_nbBars; barInSource++) {
					int barInDestination = bar + barInSource - 1;
					int dur_inSource      = block_source     ->timeSignature[barInSource     ]->duration_1024th;
					int dur_inDestination = block_destination->timeSignature[barInDestination]->duration_1024th;
					if (dur_inDestination != dur_inSource) return false;
				}

				// Si on n'a trouvé aucun problème, les durées des signatures rythmiques sont compatibles
				return true;

			}
			int writeSubFct_getDurationAfterBarIndexAndTime_1024th(BarIndexAndTime^ time) {
				
				int sum = 0;

				for (int bar = time->bar; bar <= _nbBars; bar++) {
					Signature^ timeSignature = _timeSignatures[bar];
					if (timeSignature == nullptr) throw gcnew Exception("Impossible to get the duration: at least one time signature is still undefined.");
					sum += timeSignature->duration_1024th;
				}

				sum -= time->timeInBar_1024th;

				return sum;

			}
			static Block^ writeSubFct_getBlockWithReducedLength(Block^ block, int newBlockDuration_1024th) {
				
				// Déterminer le point temporel de coupure
				BarIndexAndTime^ cutPoint = BarIndexAndTime::getInBlock(block, newBlockDuration_1024th);

				// Dupliquer le bloc, en ne gardant que les mesures jusqu'au point de coupure
				Block^ ans = block->extractBars(1, cutPoint->bar);

				// Est-il nécessaire de rogner la dernière mesure ?
				int tooMuch_1024th = block->timeSignature[cutPoint->bar]->duration_1024th - cutPoint->timeInBar_1024th;
				if (tooMuch_1024th < 0) throw gcnew Exception;
				if (tooMuch_1024th > 0) {
					for (int part = 1; part <= block->_nbParts; part++) {
						block->_contents[part, cutPoint->bar] = writeSubFct_reduceBarContentLength(block->_contents[part, cutPoint->bar], cutPoint->timeInBar_1024th);
					}
				}

				// Retourner
				return ans;

			}
			static Group^ writeSubFct_reduceBarContentLength(Group^ barContents, int newDuration_1024th) {
				
				// Récupérer le tableau des éléments
				array<Element^, 1>^ elements = barContents->elements;
				int nbElements = elements->Length;

				// Parcourir pour déterminer le premier élément qui est à supprimer ou à couper
				int index_cut;
				int cutInElementAfter_1024th;
				{
					index_cut = -1;
					int currentTime = 0;
					for (int i = 0; i < nbElements; i++) {
						int elementDuration = elements[i]->duration_1024th;
						int next_currentTime = currentTime + elementDuration;
						if (next_currentTime > newDuration_1024th) {
							index_cut = i;
							cutInElementAfter_1024th = newDuration_1024th - currentTime;
						}
						currentTime = next_currentTime;
					}
					if (index_cut == -1) return barContents;
				}

				// Créer une liste d'éléments, contenant tous les éléments avant 'index_cut'
				List<Element^>^ new_elements = gcnew List<Element^>; {
					for (int i = 0; i < index_cut; i++) {
						new_elements->Add(elements[i]);
					}
				}

				// Si l'élément à l'index 'index_cut' est à couper, ajouter la partie gauche de son découpage
				if (cutInElementAfter_1024th > 0) {
					array<Element^, 1>^ cutElements_left = elements[index_cut]->cutAt(cutInElementAfter_1024th)->left;
					for each (Element^ element in cutElements_left) {
						new_elements->Add(element);
					}
				}

				// Retourner
				return gcnew Group(new_elements);

			}
			void writeSubFct_writePart(Block^ blockToAdd, int part_source, int part_destination, int bar, int timeInBar_1024th, NotesOverwriteBehaviour^ behaviour) {

				// Récupérer la liste des éléments à ajouter
				List<Element^>^ elementsToAdd = gcnew List<Element^>; {
					int blockToAdd_nbBars = blockToAdd->nbBars;
					for (int bar = 1; bar <= blockToAdd_nbBars; bar++) {
						for each (Element^ element in blockToAdd[part_source, bar]->elements) {
							elementsToAdd->Add(element);
						}
					}
				}

				// Les ajouter
				writeSubFct_writeElementsAt(elementsToAdd, part_destination, bar, timeInBar_1024th, behaviour);

			}
			ref struct Pos {
				
				public:
					int bar;
					int indexInBar;
					int whenAtIndex_1024th;

				public:
					Pos(int bar, int indexInBar, int whenAtIndex_1024th) :
						bar               (bar               ),
						indexInBar        (indexInBar        ),
						whenAtIndex_1024th(whenAtIndex_1024th)
					{ }
					Pos(Pos^ pos) :
						bar               (pos->bar               ),
						indexInBar        (pos->indexInBar        ),
						whenAtIndex_1024th(pos->whenAtIndex_1024th)
					{ }

				public:
					void addTime(Block^ block, int incr_1024th, int part) {

						whenAtIndex_1024th += incr_1024th;
						
						while (whenAtIndex_1024th >= block->_contents[part, bar][indexInBar]->duration_1024th) {
							whenAtIndex_1024th -= block->_contents[part, bar][indexInBar]->duration_1024th;
							indexInBar++;
							if (indexInBar >= block->_contents[part, bar]->nbElements) {
								bar += 1;
								indexInBar = 0;
							}
						}

					}

			};
			void writeSubFct_writeElementsAt(List<Element^>^ elementsToAdd, int part, int bar, int timeInBar_1024th, NotesOverwriteBehaviour^ behaviour) {
				
				// Déterminer la position initiale, qui sera ensuite incrémentée à chaque ajout
				Pos^ pos; {
					int cptTimeInBar = 0;
					Group^ barContent = _contents[part, bar];
					if ((Object^)(barContent) == nullptr) {
						barContent = gcnew Group(Rest::getSum(_timeSignatures[bar]->duration_1024th));
						_contents[part, bar] = barContent;
					}
					int nbElementsInBarContent = barContent->nbElements;
					for (int i = 0; i < nbElementsInBarContent; i++) {
						int thisElementDuration = barContent[i]->duration_1024th;
						cptTimeInBar += thisElementDuration;
						if (cptTimeInBar > timeInBar_1024th) {
							pos = gcnew Pos(bar, i, cptTimeInBar - thisElementDuration);
							break;
						}
					}
				}

				// Parcourir les éléments en les ajoutant au bloc et en incrémentant la position d'ajout
				for each (Element^ elementToAdd in elementsToAdd) {
					// Ajouter l'élément au bloc et incrémenter la position
					pos = writeSubFct_writeElementAt_returnIncrementedPos(elementToAdd, part, pos, behaviour);
				}

			}
			Pos^ writeSubFct_writeElementAt_returnIncrementedPos(Element^ elementToAdd, int part, Pos^ posBeg, NotesOverwriteBehaviour^ behaviour) {

				// Déterminer la position de fin dans le bloc
				Pos^ posEnd; {
					posEnd = gcnew Pos(posBeg);
					posEnd->addTime(this, elementToAdd->duration_1024th, part);
				}
				
				// 



				// Si l'option 'requestNoCollisions' est activée, parcourir les éléments du bloc entre 'posBeg' et 'posEnd' pour vérifier
				//if ()









					throw gcnew Exception("à implémenter"); // et permettre de faire ça depuis l'extérieur (sans retourner la position) ?

				// Retourner la position incrémentée
				return posEnd;

			}

		// Autres fonctions publiques
		public:
			void transposition(double semitones) {
			
				for (int part = 1; part <= _nbParts; part++) {
					for (int bar = 1; bar <= _nbBars; bar++) {
						_contents[part, bar]->transposition(semitones);
					}
				}

			}
			int getDuration_1024th() {
				
				int sum = 0;

				for (int bar = 1; bar <= _nbBars; bar++) {
					Signature^ timeSignature = _timeSignatures[bar];
					if (timeSignature == nullptr) throw gcnew Exception("Impossible to get the duration: at least one time signature is still undefined.");
					sum += timeSignature->duration_1024th;
				}

				return sum;

			}

	};

}
}
