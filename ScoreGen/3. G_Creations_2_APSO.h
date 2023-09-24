#pragma region head

	#pragma once

	#include "OF_Combinations.h"

#pragma endregion

// Créations 2 : APSO
public ref class G_APSO_Sierpinski_common abstract : public Generator {
	
	// Types
	public:
		ref struct DynamicAndPosition {

			public:
				String^ dynamic;
				int position;
	
			public:
				DynamicAndPosition(String^ dynamic, int position) :
					dynamic(dynamic),
					position(position)
				{ }

		};

	// Options
	protected:
		int nbLayers;
		int nbNotes;
		int midiMin;
		int eightsPerBar;
		int midiMax;
		int nbBars;

	// Constructeur
	protected:
		G_APSO_Sierpinski_common(int nbLayers, int nbNotes, int midiMin, int eightsPerBar) :
			nbLayers    (nbLayers    ),
			nbNotes     (nbNotes     ),
			midiMin     (midiMin     ),
			eightsPerBar(eightsPerBar),
			midiMax     (midiMax = midiMin + nbLayers - 1),
			nbBars      (nbBars = nbNotes / eightsPerBar)
		{ }

	// Implémentations
	public:
		virtual ScoreAndInfos^ generate() override {

			// Calculer la grille des notes
			array<bool, 2>^ grid = calcGrid();

			// Choisir quelles couches sont prises en charge par quels instruments
			array<array<int, 1>^, 1>^ layersForInstruments = getLayersForInstruments();
			int nbInstruments = layersForInstruments->Length;
			{
				String^ error = tryToFindErrorInLayers(layersForInstruments, nbInstruments);
				if (error != "") {
					throw gcnew Exception("Erreur en choisissant quelles couches sont prises en charge par quels instruments :\r\n\r\n'" + error + "'");
				}
			}

			// Calculer les notes pour chaque instrument
			array<array<int, 1>^, 2>^ notesForInstruments = calcNotesForInstruments(nbInstruments, grid, layersForInstruments);
			array<bool, 2>^ notesForInstrumentsExist = calcNotesForInstrumentsExist(nbInstruments, notesForInstruments);

			// Calculer les nuances
			array<String^, 2>^ dynamicsGrid = calcDynamicsGrid(grid, nbInstruments, notesForInstrumentsExist);

			// Créer le bloc
			Block^ block = gcnew Block(nbInstruments);
			block->addBars(nbBars, "C");

			// Ajouter le contenu
			for (int part = 1; part <= nbInstruments; part++) {
				for (int bar = 1; bar <= nbBars; bar++) {
					List<Element^>^ elements = gcnew List<Element^>;
					fillBarContents(elements, part, bar, notesForInstruments, notesForInstrumentsExist, dynamicsGrid);
					if (elements->Count > 0) block[part, bar] = gcnew Group(elements);
				}
			}
			
			// Ajouter une image et des commentaires
			Bitmap^ image = calcImage(grid, dynamicsGrid, layersForInstruments, nbInstruments);
			String^ comments = getComments();
			
			// Retourner
			return gcnew ScoreAndInfos(block, image, comments);
			
		}
		
	// Fonctions
	protected:
		array<bool, 2>^ getSierpinski(array<int, 2>^ initial, int h, int w, int modulo, bool toric, bool reverseX, bool reverseY, array<int, 1>^ trueValues) {
			
			// Créer le tableau de réponse en recopiant les valeurs initiales, et vérifier les options
			array<int, 2>^ sierp = gcnew array<int, 2>(h, w); {
				if (initial == nullptr) {
					throw gcnew Exception;
				} else {
					if (initial->GetUpperBound(0) + 1 != h) throw gcnew Exception;
					if (initial->GetUpperBound(1) + 1 != w) throw gcnew Exception;
					for (int i = 0; i < h; i++) {
						for (int j = 0; j < w; j++) {
							sierp[i, j] = initial[i, j];
						}
					}
				}
				if (modulo <= 1) throw gcnew Exception;
			}
				
			// Remplir
			for (int j = 1; j < w; j++) {
				for (int i = 0; i < h; i++) {
					int val1, val2;
					if (toric) {
						val1 = sierp[i, j - 1];
						val2 = (i < h - 1) ? sierp[i + 1, j - 1] : sierp[0, j - 1];
					} else {
						val1 = sierp[i, j - 1];
						val2 = (i < h - 1) ? sierp[i + 1, j - 1] : 0;
					}
					int left_i_2 = (i < nbLayers - 1) ? i + 1 : 0;
					sierp[i, j] = (val1 + val2 + initial[i, j]) % modulo;
				}
			}
			
			// Booléaniser
			array<bool, 2>^ asBool = gcnew array<bool, 2>(h, w); {
				for (int j = 0; j < w; j++) {
					for (int i = 0; i < h; i++) {
						asBool[i, j] = contains(trueValues, sierp[i, j]);
					}
				}
			}
			
			// Renverser les X si besoin
			if (reverseX) {
				for (int i = 0; i < h; i++) {
					int mid = w / 2;
					for (int j = 0; j < mid; j++) {
						int J = w - 1 - j;
						bool exchang = asBool[i, j];
						asBool[i, j] = asBool[i, J];
						asBool[i, J] = exchang;
					}
				}
			}
			
			// Renverser les Y si besoin
			if (reverseY) {
				for (int j = 0; j < w; j++) {
					int mid = h / 2;
					for (int i = 0; i < mid; i++) {
						int I = h - 1 - i;
						bool exchang = asBool[i, j];
						asBool[i, j] = asBool[I, j];
						asBool[I, j] = exchang;
					}
				}
			}

			// Récupérer les offsets verticaux (tout à 0 si on veut tout garder identique), et tout bouger vers le haut en conséquence
			{
				array<int, 1>^ verticalOffsets = getSierpinskiVerticalOffsets(w);
				for (int j = 0; j < nbLayers; j++) {
					int offset = verticalOffsets[j];
					for (int i = 0                ; i < nbLayers - offset; i++) asBool[i, j] = asBool[i + offset, j];
					for (int i = nbLayers - offset; i < nbLayers         ; i++) asBool[i, j] = false;
				}
			}

			// Retourner
			return asBool;

		}
	private:
		String^ tryToFindErrorInLayers(array<array<int, 1>^, 1>^ layersForInstruments, int nbInstruments) {
			
			// Tous les layers sont-ils pris en charge par un instrument et un seul ?
			array<int, 1>^ countHowManyTimesEachLayerIsPlayed = gcnew array<int, 1>(nbLayers);
			for (int instrument = 0; instrument < nbInstruments; instrument++) {
				for each (int layer in layersForInstruments[instrument]) {
					countHowManyTimesEachLayerIsPlayed[layer]++;
				}
			}
			for (int i = 0; i < nbLayers; i++) {
				if (countHowManyTimesEachLayerIsPlayed[i] < 1) {
					return "The layer " + i + " isn't played.";
				} else if (countHowManyTimesEachLayerIsPlayed[i] > 1) {
					return "The layer " + i + " is played " + countHowManyTimesEachLayerIsPlayed[i] + " times.";
				}
			}
			return "";

		}
		array<array<int, 1>^, 2>^ calcNotesForInstruments(int nbInstruments, array<bool, 2>^ grid, array<array<int, 1>^, 1>^ layersForInstruments) {
		
			// Créer le tableau
			array<array<int, 1>^, 2>^ ans = gcnew array<array<int, 1>^, 2>(nbInstruments, nbNotes);

			// Remplir
			for (int instrument = 0; instrument < nbInstruments; instrument++) {
				for (int time = 0; time < nbNotes; time++) {
					List<int>^ pitches = gcnew List<int>;
					for each (int layer in layersForInstruments[instrument]) {
						if (grid[layer, time]) pitches->Add(midiMax - layer);
					}
					ans[instrument, time] = pitches->ToArray();
				}
			}

			// Retourner
			return ans;

		}
		array<bool, 2>^ calcNotesForInstrumentsExist(int nbInstruments, array<array<int, 1>^, 2>^ notesForInstruments) {
			
			array<bool, 2>^ ans = gcnew array<bool, 2>(nbInstruments, nbNotes);

			for (int instrument = 0; instrument < nbInstruments; instrument++) {
				for (int time = 0; time < nbNotes; time++) {
					ans[instrument, time] = notesForInstruments[instrument, time]->Length > 0;
				}
			}

			return ans;

		}
		array<String^, 1>^ getDynamicsInTime(array<DynamicAndPosition^, 1>^ globalDynamicsChanges) {
			
			array<String^, 1>^ ans = gcnew array<String^, 1>(nbNotes);

			for (int i = 0; i < globalDynamicsChanges->Length; i++) {
				String^ currentDynamic = globalDynamicsChanges[i]->dynamic;
				int nextPos = (i < globalDynamicsChanges->Length - 1) ? globalDynamicsChanges[i + 1]->position : nbNotes;
				for (int j = globalDynamicsChanges[i]->position; j < nextPos; j++) {
					ans[j] = currentDynamic;
				}
			}

			return ans;

		}
		array<String^, 2>^ calcDynamicsGrid(array<bool, 2>^ grid, int nbInstruments, array<bool, 2>^ notesForInstrumentsExist) {
	
			// Positionner les dynamiques globales
			array<DynamicAndPosition^, 1>^ globalDynamicsChanges = getGlobalDynamicsChanges(grid);
			array<String^, 1>^ dynamicsInTime = getDynamicsInTime(globalDynamicsChanges);

			// Remplir la grille des nuances
			array<String^, 2>^ ans = gcnew array<String^, 2>(nbLayers, nbNotes);
			for (int instrument = 0; instrument < nbInstruments; instrument++) {
				// Remplir à vide la nuance de chaque case
				for (int time = 0; time < nbNotes; time++) ans[instrument, time] = "";
				// Ajouter la première nuance quand l'instrument entre (en respectant la nuance globale)
				{
					int firstTime = 0;
					while (firstTime < nbNotes && !notesForInstrumentsExist[instrument, firstTime]) firstTime++;
					if (firstTime < nbNotes) ans[instrument, firstTime] = dynamicsInTime[firstTime];
				}
				// Ajouter une nuance à chaque changement général
				for (int dynamicChangeIndex = 1; dynamicChangeIndex < globalDynamicsChanges->Length; dynamicChangeIndex++) {
					String^ newDynamic = globalDynamicsChanges[dynamicChangeIndex]->dynamic;
					int     changePos  = globalDynamicsChanges[dynamicChangeIndex]->position;
					while (changePos != nbNotes && !notesForInstrumentsExist[instrument, changePos]) changePos++;
					if (changePos < nbNotes) {
						ans[instrument, changePos] = newDynamic;
					}
				}
			}

			// Retourner
			return ans;

		}
		void fillBarContents(List<Element^>^ elements_out, int part, int bar, array<array<int, 1>^, 2>^ notesForInstruments, array<bool, 2>^ notesForInstrumentsExist, array<String^, 2>^ dynamics) {
			
			// Si la mesure est vide pour cet instrument, arrêter directement pour laisser une pause de mesure
			{
				bool isVoid = true;
				for (int timeInBar = 0; timeInBar < eightsPerBar; timeInBar++) {
					int time = (bar - 1) * eightsPerBar + timeInBar;
					if (notesForInstrumentsExist[part - 1, time]) {
						isVoid = false;
						break;
					}
				}
				if (isVoid) return;
			}

			// Parcourir le temps dans la mesure
			int timeInBar = 0;
			while (timeInBar < eightsPerBar) {
				int time = (bar - 1) * eightsPerBar + timeInBar;
				NoteOrRest^ elementToAdd;
				int timeIncrement;
				// La case est-elle pleine ?
				if (notesForInstrumentsExist[part - 1, time]) {
					// L'élément est une note : durée de 1 (incrément temporel idem)
					timeIncrement = 1;
					// Créer la note ou l'accord
					{
						List<Pitch^>^ pitches = gcnew List<Pitch^>;
						for each (int midiPitch in notesForInstruments[part - 1, time]) pitches->Add(gcnew Pitch(midiPitch));
						List<Words::Words^>^ texts = gcnew List<Words::Words^>;
						if (dynamics[part - 1, time] != "") texts->Add(qWords(dynamics[part - 1, time]));
						{
							// Première note de cet instrument ?
							bool isFirstNote = true;
							for (int i = time - 1; i >= 0; i--) {
								if (notesForInstrumentsExist[part - 1, i]) {
									isFirstNote = false;
									break;
								}
							}
							// Si oui, indiquer le mode de jeu
							if (isFirstNote) texts->Add(qWords("." + getPlayingMode()));
						}
						elementToAdd = gcnew Note(RhythmFigure::_eighth, pitches->ToArray(), texts->ToArray());
					}
				} else {
					// L'élément est un silence : déterminer sa durée (servant aussi d'incrément temporel)
					{
						// Déterminer la durée de silence dans la mesure à partir de ce point
						int nbSilence = 1;
						for (int i = timeInBar + 1; i < eightsPerBar; i++) {
							if (notesForInstrumentsExist[part - 1, (bar - 1) * eightsPerBar + i]) break;
							nbSilence++;
						}
						// Conclure la durée effective du silence (incrément temporel)
						if      (nbSilence >= 8 && eightsPerBar % 8 == 0 && timeInBar % 8 == 0) timeIncrement = 8;
						else if (nbSilence >= 4 && eightsPerBar % 4 == 0 && timeInBar % 4 == 0) timeIncrement = 4;
						else if (nbSilence >= 2 && eightsPerBar % 2 == 0 && timeInBar % 2 == 0) timeIncrement = 2;
						else                                                                    timeIncrement = 1;
					}
					// Créer le silence
					if      (timeIncrement == 8) elementToAdd = gcnew Rest(RhythmFigure::_whole  );
					else if (timeIncrement == 4) elementToAdd = gcnew Rest(RhythmFigure::_half   );
					else if (timeIncrement == 2) elementToAdd = gcnew Rest(RhythmFigure::_quarter);
					else if (timeIncrement == 1) elementToAdd = gcnew Rest(RhythmFigure::_eighth );
					else throw gcnew Exception;
				}
				// Ajouter l'élément et incrémenter le temps
				elements_out->Add(elementToAdd);
				timeInBar += timeIncrement;
			}

		}
	private:
		Bitmap^ calcImage(array<bool, 2>^ grid, array<String^, 2>^ dynamics, array<array<int, 1>^, 1>^ layersForInstruments, int nbInstruments) {
			
			int pixelsPerCell = 10;
			int w = pixelsPerCell * nbNotes;
			int h = pixelsPerCell * nbLayers;
			
			// Création de l'image
			Bitmap^ ans = gcnew Bitmap(w, h);
			Graphics^ g = Graphics::FromImage(ans);
			g->Clear(Color::White);
			
			// Dessiner les barres de mesures
			{
				Pen^ pen_bars = gcnew Pen(Color:: FromArgb(255, 215, 175));
				pen_bars->DashStyle = DashStyle::Dot;
				for (int i = eightsPerBar; i <= nbNotes - 1; i += eightsPerBar) {
					int x = i * pixelsPerCell;
					g->DrawLine(pen_bars, x, 0, x, h - 1);
				}
			}
			
			// Dessiner les notes
			{
				// Création des brushes pour les notes
				Dictionary<String^, Brush^>^ brushes = gcnew Dictionary<String^, Brush^>; {
					brushes["!ppp"] = gcnew SolidBrush(Color::FromArgb(100, 100, 100)); // ...   
					brushes["!pp" ] = gcnew SolidBrush(Color::FromArgb(  0,   0,   0)); // noir  
					brushes["!p"  ] = gcnew SolidBrush(Color::FromArgb( 85,  42,   0)); // ...   
					brushes["!mp" ] = gcnew SolidBrush(Color::FromArgb(170,  84,   0)); // ...   
					brushes["!mf" ] = gcnew SolidBrush(Color::FromArgb(255, 127,   0)); // orange
					brushes["!f"  ] = gcnew SolidBrush(Color::FromArgb(255, 192,   0)); // ...   
					brushes["!ff" ] = gcnew SolidBrush(Color::FromArgb(255, 255,   0)); // jaune 
				}
				// Dessin des notes
				for (int layer = 0; layer < nbLayers; layer++) {
					// Trouver quel instrument joue ce layer
					int instrument; {
						for (int instr = 0; instr < nbInstruments; instr++) {
							bool contains = false;
							for each (int llayer in layersForInstruments[instr]) {
								if (llayer == layer) {
									contains = true;
									break;
								}
							}
							if (contains) {
								instrument = instr;
								break;
							}
						}
					}
					// Dessiner les notes
					for (int time = 0; time < nbNotes; time++) {
						if (grid[layer, time]) {
							int timeLastDynamic = time;
							while (timeLastDynamic >= 0 && dynamics[instrument, timeLastDynamic] == "") timeLastDynamic--;
							String^ dynamic = dynamics[instrument, timeLastDynamic];
							Brush^ brush = (timeLastDynamic >= 0 && dynamic != nullptr && brushes->ContainsKey(dynamic)) ? brushes[dynamic] : Brushes::Blue;
							g->FillEllipse(brush, pixelsPerCell * time, pixelsPerCell * layer, pixelsPerCell, pixelsPerCell);
						}
					}
				}
			}

			// Retour
			return ans;

		}

	// A implémenter
	protected:
		virtual array<array<int, 1>^, 1>^ getLayersForInstruments() = 0;
		virtual array<bool, 2>^ calcGrid() = 0;
		virtual array<int, 1>^ getSierpinskiVerticalOffsets(int w) = 0;
		virtual array<DynamicAndPosition^, 1>^ getGlobalDynamicsChanges(array<bool, 2>^ grid) = 0;
		virtual String^ getPlayingMode() = 0;
		virtual String^ getComments() = 0;

};
public ref class G_APSO_Sierpinski_1_Strings : public G_APSO_Sierpinski_common {

	// Constructeur
	public:
		G_APSO_Sierpinski_1_Strings() :
			G_APSO_Sierpinski_common(
				69,	// nbLayers
				128,// nbNotes
				30,	// midiMin
				8	// eightsPerBar
			)
		{ }

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "APSO : Sierpinski 1 (cordes)";

		}
		virtual String^ description() override {
			
			return "Croissance de Sierpinski : chaque case noire est jouée comme une note. Répartition pour 8.7.6.5.3 cordes jouant en doubles cordes.";

		}
	protected:
		virtual array<array<int, 1>^, 1>^ getLayersForInstruments() override {
			
			int nbInstruments = 29;
			array<array<int, 1>^, 1>^ ans = gcnew array<array<int, 1>^, 1>(nbInstruments);
			int cpt = 0;

			// Violons 1
			ans[cpt++] = gcnew array<int, 1>{ 4, 13 };
			ans[cpt++] = gcnew array<int, 1>{ 5, 15 };
			ans[cpt++] = gcnew array<int, 1>{ 7, 16 };
			ans[cpt++] = gcnew array<int, 1>{ 6, 17 };
			ans[cpt++] = gcnew array<int, 1>{ 0, 9, 18 };
			ans[cpt++] = gcnew array<int, 1>{ 1, 10, 19 };
			ans[cpt++] = gcnew array<int, 1>{ 2, 11, 20 };
			ans[cpt++] = gcnew array<int, 1>{ 3, 12, 21 };

			// Violons 2
			ans[cpt++] = gcnew array<int, 1>{ 8, 14, 24 };
			ans[cpt++] = gcnew array<int, 1>{ 29, 30 };
			ans[cpt++] = gcnew array<int, 1>{ 23, 33 };
			ans[cpt++] = gcnew array<int, 1>{ 22, 26, 34 };
			ans[cpt++] = gcnew array<int, 1>{ 25, 36 };
			ans[cpt++] = gcnew array<int, 1>{ 28, 31, 37 };
			ans[cpt++] = gcnew array<int, 1>{ 27, 32, 38 };

			// Altos
			ans[cpt++] = gcnew array<int, 1>{ 35, 44 };
			ans[cpt++] = gcnew array<int, 1>{ 43, 46 };
			ans[cpt++] = gcnew array<int, 1>{ 41, 47 };
			ans[cpt++] = gcnew array<int, 1>{ 39, 48 };
			ans[cpt++] = gcnew array<int, 1>{ 40, 49 };
			ans[cpt++] = gcnew array<int, 1>{ 42, 50 };
			
			// Violoncelles
			ans[cpt++] = gcnew array<int, 1>{ 51, 56 };
			ans[cpt++] = gcnew array<int, 1>{ 55, 58 };
			ans[cpt++] = gcnew array<int, 1>{ 53, 59 };
			ans[cpt++] = gcnew array<int, 1>{ 52, 61 };
			ans[cpt++] = gcnew array<int, 1>{ 45, 54, 62 };

			// Contrebasses
			ans[cpt++] = gcnew array<int, 1>{ 60, 63, 66 };
			ans[cpt++] = gcnew array<int, 1>{ 64, 67 };
			ans[cpt++] = gcnew array<int, 1>{ 57, 65, 68 };

			// Retour
			return ans;

		}
		virtual array<bool, 2>^ calcGrid() override {
			
			// Tableau initial
			int h = nbLayers;
			int w = nbNotes;
			array<int, 2>^ initial = gcnew array<int, 2>(h, w);
			initial[h - 1, 0] = 1;

			// Sierpinskisation
			array<bool, 2>^ sierpinski = getSierpinski(initial, h, w, 2, true, false, false, gcnew array<int> { 1 });

			// Retourner
			return sierpinski;

		}
		virtual array<int, 1>^ getSierpinskiVerticalOffsets(int w) override {
			
			array<int, 1>^ ans = gcnew array<int, 1>(w);

			for (int i = 0; i < w; i++) ans[i] = 0;

			return ans;

		}
		virtual array<DynamicAndPosition^, 1>^ getGlobalDynamicsChanges(array<bool, 2>^ grid) override {

			// Créer la liste de dynamiques
			List<DynamicAndPosition^>^ ans = gcnew List<DynamicAndPosition^>;
			
			// Parties temporelles du morceau
			int part1_beg = 0;
			int part1_end = (int)Math::Round(Math::Pow(2, Math::Floor(Math::Round(Math::Log(nbLayers, 2), 9)))) - 1;
			int part2_beg = part1_end + 1;
			int part2_end = nbNotes - 1;
			
			// Remplir la liste de dynamiques
			ans->Add(gcnew DynamicAndPosition("!pp", part1_beg + (part1_end - part1_beg) *  0 / 11));
			ans->Add(gcnew DynamicAndPosition("!p" , part1_beg + (part1_end - part1_beg) *  2 / 11));
			ans->Add(gcnew DynamicAndPosition("!mp", part1_beg + (part1_end - part1_beg) *  4 / 11));
			ans->Add(gcnew DynamicAndPosition("!mf", part1_beg + (part1_end - part1_beg) *  6 / 11));
			ans->Add(gcnew DynamicAndPosition("!f" , part1_beg + (part1_end - part1_beg) *  8 / 11));
			ans->Add(gcnew DynamicAndPosition("!ff", part1_beg + (part1_end - part1_beg) * 10 / 11));
			ans->Add(gcnew DynamicAndPosition("!pp", part2_beg + (part2_end - part2_beg) *  0 / 21));
			ans->Add(gcnew DynamicAndPosition("!p" , part2_beg + (part2_end - part2_beg) *  6 / 21));
			ans->Add(gcnew DynamicAndPosition("!mp", part2_beg + (part2_end - part2_beg) * 11 / 21));
			ans->Add(gcnew DynamicAndPosition("!mf", part2_beg + (part2_end - part2_beg) * 15 / 21));
			ans->Add(gcnew DynamicAndPosition("!f" , part2_beg + (part2_end - part2_beg) * 18 / 21));
			ans->Add(gcnew DynamicAndPosition("!ff", part2_beg + (part2_end - part2_beg) * 20 / 21));
			
			// Retourner sous forme de tableau
			return ans->ToArray();
			
		}
		virtual String^ getPlayingMode() override {
			
			return "pizz. (près du chevalet)";

		}
		virtual String^ getComments() override {
			
			return
				"Signature C" + "\r\n" +
				"Tempo noire = 100" + "\r\n" +
				"Echanges de parties dans les v1 vers mesure 9 pour que le bloc aigu se déplace du fond vers l'avant";
		}

};
public ref class G_APSO_Sierpinski_2_Strings : public G_APSO_Sierpinski_common {

	// Constructeur
	public:
		G_APSO_Sierpinski_2_Strings() :
			G_APSO_Sierpinski_common(
				64,	// nbLayers
				114,// nbNotes
				30,	// midiMin
				8	// eightsPerBar
			)
		{ }

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "APSO : Sierpinski 2 (cordes)";

		}
		virtual String^ description() override {
			
			return "Croissance symétrique de Sierpinski, partant du médium. Répartition pour 8.7.6.5.3 cordes jouant en doubles cordes.";

		}
	protected:
		virtual array<array<int, 1>^, 1>^ getLayersForInstruments() override {
			
			int nbInstruments = 29;
			array<array<int, 1>^, 1>^ ans = gcnew array<array<int, 1>^, 1>(nbInstruments);
			int cpt = 0;

			// Violons 1
			ans[cpt++] = gcnew array<int, 1>{ 5 };
			ans[cpt++] = gcnew array<int, 1>{ 6 };
			ans[cpt++] = gcnew array<int, 1>{ 0, 9 };
			ans[cpt++] = gcnew array<int, 1>{ 1, 10 };
			ans[cpt++] = gcnew array<int, 1>{ 2, 11 };
			ans[cpt++] = gcnew array<int, 1>{ 3, 12 };
			ans[cpt++] = gcnew array<int, 1>{ 7, 16 };
			ans[cpt++] = gcnew array<int, 1>{ 8, 19 };

			// Violons 2
			ans[cpt++] = gcnew array<int, 1>{ 18 };
			ans[cpt++] = gcnew array<int, 1>{ 17, 20 };
			ans[cpt++] = gcnew array<int, 1>{ 15, 21 };
			ans[cpt++] = gcnew array<int, 1>{ 4, 13, 22 };
			ans[cpt++] = gcnew array<int, 1>{ 14, 23 };
			ans[cpt++] = gcnew array<int, 1>{ 24, 32 };
			ans[cpt++] = gcnew array<int, 1>{ 31, 38 };

			// Altos
			ans[cpt++] = gcnew array<int, 1>{ 30, 39 };
			ans[cpt++] = gcnew array<int, 1>{ 26, 34, 42 };
			ans[cpt++] = gcnew array<int, 1>{ 25, 33, 41 };
			ans[cpt++] = gcnew array<int, 1>{ 27, 35, 43 };
			ans[cpt++] = gcnew array<int, 1>{ 28, 36, 44 };
			ans[cpt++] = gcnew array<int, 1>{ 29, 37, 45 };
			
			// Violoncelles
			ans[cpt++] = gcnew array<int, 1>{ 47, 56 };
			ans[cpt++] = gcnew array<int, 1>{ 40, 49, 57 };
			ans[cpt++] = gcnew array<int, 1>{ 48, 54 };
			ans[cpt++] = gcnew array<int, 1>{ 50, 53 };
			ans[cpt++] = gcnew array<int, 1>{ 46, 51 };

			// Contrebasses
			ans[cpt++] = gcnew array<int, 1>{ 55, 58, 61 };
			ans[cpt++] = gcnew array<int, 1>{ 59, 62 };
			ans[cpt++] = gcnew array<int, 1>{ 52, 60, 63 };

			// Retour
			return ans;

		}
		virtual array<bool, 2>^ calcGrid() override {
			
			// Tableau initial
			int h = nbLayers;
			int w = nbLayers;
			array<int, 2>^ initial = gcnew array<int, 2>(h, w);
			initial[h - 1, 0] = 1;

			// Sierpinskisation
			array<bool, 2>^ sierpinski = getSierpinski(initial, h, w, 2, true, false, false, gcnew array<int> { 1 });
			
			// Calcul du tableau plus large, en commençant par recopier le premier
			int H = nbLayers;
			int W = nbNotes;
			array<bool, 2>^ ans = gcnew array<bool, 2>(H, W);
			for (int i = 0; i < h; i++) {
				for (int j = 0; j < w; j++) {
					ans[i, j] = sierpinski[i, j];
				}
			}

			// Blocs avec des trous
			for (int j = nbLayers; j < nbNotes; j++) {
				for (int i = 0; i < nbLayers; i++) {
					ans[i, j] = true;
				}
			}
			array<int, 1>^ holes = gcnew array<int, 1>{ nbLayers + 8, nbLayers + 19 };
			for each (int j in holes) {
				for (int i = 0; i < nbLayers; i++) {
					ans[i, j    ] = false;
					ans[i, j + 1] = false;
				}
			}
			for (int j = holes[0]; j < holes[1]; j++) {
				for (int i = 0; i < (int)((double)nbLayers * 0.2); i++) ans[i, j] = false;
				for (int i = (int)((double)nbLayers * 0.75); i < nbLayers; i++) ans[i, j] = false;
			}
			for (int j = holes[1]; j < nbNotes; j++) {
				for (int i = (int)((double)nbLayers * 0.35); i < (int)((double)nbLayers * 0.75); i++) ans[i, j] = false;
			}
			
			// Retour
			return ans;

		}
		virtual array<int, 1>^ getSierpinskiVerticalOffsets(int w) override {
			
			array<int, 1>^ ans = gcnew array<int, 1>(w);

			double offsetInit = 11;
			double offsetMid = 23;
			double end = nbLayers - 1;
			double sqr = Math::Sqrt(1 - offsetInit / (offsetInit - offsetMid));
			double c = offsetInit;
			double b1 = (-1 - sqr) * (2 * (offsetInit - offsetMid)) / end;
			double b2 = (-1 + sqr) * (2 * (offsetInit - offsetMid)) / end;
			double a1 = b1 * b1 / (4 * (offsetInit - offsetMid));
			double a2 = b2 * b2 / (4 * (offsetInit - offsetMid));
			double a, b;
			double posExtr1 = -0.5 * b1 / a1;
			if (posExtr1 >= 0 && posExtr1 <= end) {
				a = a1;
				b = b1;
			} else {
				a = a2;
				b = b2;
			}
			
			for (int i = 0; i < w; i++) {
				ans[i] = (int)Math::Round(a * i * i + b * i + c);
			}

			return ans;

		}
		virtual array<DynamicAndPosition^, 1>^ getGlobalDynamicsChanges(array<bool, 2>^ grid) override {

			// Créer la liste de dynamiques
			List<DynamicAndPosition^>^ ans = gcnew List<DynamicAndPosition^>;
			
			// Parties temporelles du morceau
			int beg = 0;
			int end = nbLayers - 1;

			// Remplir la liste de dynamiques
			ans->Add(gcnew DynamicAndPosition("!pp", beg + (end - beg) *  0 / 36));
			ans->Add(gcnew DynamicAndPosition("!p" , beg + (end - beg) * 11 / 36));
			ans->Add(gcnew DynamicAndPosition("!mp", beg + (end - beg) * 20 / 36));
			ans->Add(gcnew DynamicAndPosition("!mf", beg + (end - beg) * 27 / 36));
			ans->Add(gcnew DynamicAndPosition("!f" , beg + (end - beg) * 32 / 36));
			ans->Add(gcnew DynamicAndPosition("!ff", beg + (end - beg) * 35 / 36));
			int firstHole = nbLayers;
			while (grid[0, firstHole]) firstHole++;
			ans->Add(gcnew DynamicAndPosition("!ppp", firstHole + 1));

			// Retourner sous forme de tableau
			return ans->ToArray();
			
		}
		virtual String^ getPlayingMode() override {
			
			return "pizz. (résonant)";

		}
		virtual String^ getComments() override {
			
			return
				"Signature C" + "\r\n" +
				"Tempo noire = 120";
		}

};
public ref class G_APSO_Sierpinski_3_Strings : public G_APSO_Sierpinski_common {

	// Constructeur
	public:
		G_APSO_Sierpinski_3_Strings() :
			G_APSO_Sierpinski_common(
				69,	// nbLayers
				128,// nbNotes
				30,	// midiMin
				8	// eightsPerBar
			)
		{ }

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "APSO : Sierpinski 3 (cordes)";

		}
		virtual String^ description() override {
			
			return "Croissance de Sierpinski descendante.";

		}
	protected:
		virtual array<array<int, 1>^, 1>^ getLayersForInstruments() override {
			
			int nbInstruments = 29;
			array<array<int, 1>^, 1>^ ans = gcnew array<array<int, 1>^, 1>(nbInstruments);
			int cpt = 0;

			// Violons 1
			ans[cpt++] = gcnew array<int, 1>{ 4, 13 };
			ans[cpt++] = gcnew array<int, 1>{ 5, 15 };
			ans[cpt++] = gcnew array<int, 1>{ 7, 16 };
			ans[cpt++] = gcnew array<int, 1>{ 6, 17 };
			ans[cpt++] = gcnew array<int, 1>{ 0, 9, 18 };
			ans[cpt++] = gcnew array<int, 1>{ 1, 10, 19 };
			ans[cpt++] = gcnew array<int, 1>{ 2, 11, 20 };
			ans[cpt++] = gcnew array<int, 1>{ 3, 12, 21 };

			// Violons 2
			ans[cpt++] = gcnew array<int, 1>{ 8, 14, 24 };
			ans[cpt++] = gcnew array<int, 1>{ 29, 30 };
			ans[cpt++] = gcnew array<int, 1>{ 23, 33 };
			ans[cpt++] = gcnew array<int, 1>{ 22, 26, 34 };
			ans[cpt++] = gcnew array<int, 1>{ 25, 36 };
			ans[cpt++] = gcnew array<int, 1>{ 28, 31, 37 };
			ans[cpt++] = gcnew array<int, 1>{ 27, 32, 38 };

			// Altos
			ans[cpt++] = gcnew array<int, 1>{ 35, 44 };
			ans[cpt++] = gcnew array<int, 1>{ 43, 46 };
			ans[cpt++] = gcnew array<int, 1>{ 41, 47 };
			ans[cpt++] = gcnew array<int, 1>{ 39, 48 };
			ans[cpt++] = gcnew array<int, 1>{ 40, 49 };
			ans[cpt++] = gcnew array<int, 1>{ 42, 50 };
			
			// Violoncelles
			ans[cpt++] = gcnew array<int, 1>{ 51, 56 };
			ans[cpt++] = gcnew array<int, 1>{ 55, 58 };
			ans[cpt++] = gcnew array<int, 1>{ 53, 59 };
			ans[cpt++] = gcnew array<int, 1>{ 52, 61 };
			ans[cpt++] = gcnew array<int, 1>{ 45, 54, 62 };

			// Contrebasses
			ans[cpt++] = gcnew array<int, 1>{ 60, 63, 66 };
			ans[cpt++] = gcnew array<int, 1>{ 64, 67 };
			ans[cpt++] = gcnew array<int, 1>{ 57, 65, 68 };

			// Retour
			return ans;

		}
		virtual array<bool, 2>^ calcGrid() override {
			
			// Tableau initial
			int h = nbLayers;
			int w = nbNotes;
			array<int, 2>^ initial = gcnew array<int, 2>(h, w);
			initial[h - 1, 0] = 1;

			// Sierpinskisation
			array<bool, 2>^ sierpinski = getSierpinski(initial, h, w, 2, true, false, true, gcnew array<int> { 1 });

			// Retourner
			return sierpinski;

		}
		virtual array<int, 1>^ getSierpinskiVerticalOffsets(int w) override {
			
			array<int, 1>^ ans = gcnew array<int, 1>(w);

			for (int i = 0; i < w; i++) ans[i] = 0;

			return ans;

		}
		virtual array<DynamicAndPosition^, 1>^ getGlobalDynamicsChanges(array<bool, 2>^ grid) override {

			// Créer la liste de dynamiques
			List<DynamicAndPosition^>^ ans = gcnew List<DynamicAndPosition^>;
			
			// Parties temporelles du morceau
			int part1_beg = 0;
			int part1_end = (int)Math::Round(Math::Pow(2, Math::Floor(Math::Round(Math::Log(nbLayers, 2), 9)))) - 1;
			int part2_beg = part1_end + 1;
			int part2_end = nbNotes - 1;
			
			// Remplir la liste de dynamiques
			ans->Add(gcnew DynamicAndPosition("!pp", part1_beg + (part1_end - part1_beg) *  0 / 11));
			ans->Add(gcnew DynamicAndPosition("!p" , part1_beg + (part1_end - part1_beg) *  2 / 11));
			ans->Add(gcnew DynamicAndPosition("!mp", part1_beg + (part1_end - part1_beg) *  4 / 11));
			ans->Add(gcnew DynamicAndPosition("!mf", part1_beg + (part1_end - part1_beg) *  6 / 11));
			ans->Add(gcnew DynamicAndPosition("!f" , part1_beg + (part1_end - part1_beg) *  8 / 11));
			ans->Add(gcnew DynamicAndPosition("!ff", part1_beg + (part1_end - part1_beg) * 10 / 11));
			ans->Add(gcnew DynamicAndPosition("!pp", part2_beg + (part2_end - part2_beg) *  0 / 21));
			ans->Add(gcnew DynamicAndPosition("!p" , part2_beg + (part2_end - part2_beg) *  6 / 21));
			ans->Add(gcnew DynamicAndPosition("!mp", part2_beg + (part2_end - part2_beg) * 11 / 21));
			ans->Add(gcnew DynamicAndPosition("!mf", part2_beg + (part2_end - part2_beg) * 15 / 21));
			ans->Add(gcnew DynamicAndPosition("!f" , part2_beg + (part2_end - part2_beg) * 18 / 21));
			ans->Add(gcnew DynamicAndPosition("!ff", part2_beg + (part2_end - part2_beg) * 20 / 21));
			
			// Retourner sous forme de tableau
			return ans->ToArray();
			
		}
		virtual String^ getPlayingMode() override {
			
			return "pizz. (près du chevalet)";

		}
		virtual String^ getComments() override {
			
			return
				"Signature C" + "\r\n" +
				"Tempo noire = 100";
		}

};
public ref class G_APSO_Sierpinski_4_Strings : public G_APSO_Sierpinski_common {

	// Constructeur
	public:
		G_APSO_Sierpinski_4_Strings() :
			G_APSO_Sierpinski_common(
				69,	// nbLayers
				200,// nbNotes
				30,	// midiMin
				8	// eightsPerBar
			)
		{ }

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "APSO : Sierpinski modulo 12";

		}
		virtual String^ description() override {
			
			return "Triangle de Pascal modulo 12, proposé par Jérôme.";

		}
	protected:
		virtual array<array<int, 1>^, 1>^ getLayersForInstruments() override {
			
			int nbInstruments = 29;
			array<array<int, 1>^, 1>^ ans = gcnew array<array<int, 1>^, 1>(nbInstruments);
			int cpt = 0;

			// Violons 1
			ans[cpt++] = gcnew array<int, 1>{ 4, 13 };
			ans[cpt++] = gcnew array<int, 1>{ 5, 15 };
			ans[cpt++] = gcnew array<int, 1>{ 7, 16 };
			ans[cpt++] = gcnew array<int, 1>{ 6, 17 };
			ans[cpt++] = gcnew array<int, 1>{ 0, 9, 18 };
			ans[cpt++] = gcnew array<int, 1>{ 1, 10, 19 };
			ans[cpt++] = gcnew array<int, 1>{ 2, 11, 20 };
			ans[cpt++] = gcnew array<int, 1>{ 3, 12, 21 };

			// Violons 2
			ans[cpt++] = gcnew array<int, 1>{ 8, 14, 24 };
			ans[cpt++] = gcnew array<int, 1>{ 29, 30 };
			ans[cpt++] = gcnew array<int, 1>{ 23, 33 };
			ans[cpt++] = gcnew array<int, 1>{ 22, 26, 34 };
			ans[cpt++] = gcnew array<int, 1>{ 25, 36 };
			ans[cpt++] = gcnew array<int, 1>{ 28, 31, 37 };
			ans[cpt++] = gcnew array<int, 1>{ 27, 32, 38 };

			// Altos
			ans[cpt++] = gcnew array<int, 1>{ 35, 44 };
			ans[cpt++] = gcnew array<int, 1>{ 43, 46 };
			ans[cpt++] = gcnew array<int, 1>{ 41, 47 };
			ans[cpt++] = gcnew array<int, 1>{ 39, 48 };
			ans[cpt++] = gcnew array<int, 1>{ 40, 49 };
			ans[cpt++] = gcnew array<int, 1>{ 42, 50 };
			
			// Violoncelles
			ans[cpt++] = gcnew array<int, 1>{ 51, 56 };
			ans[cpt++] = gcnew array<int, 1>{ 55, 58 };
			ans[cpt++] = gcnew array<int, 1>{ 53, 59 };
			ans[cpt++] = gcnew array<int, 1>{ 52, 61 };
			ans[cpt++] = gcnew array<int, 1>{ 45, 54, 62 };

			// Contrebasses
			ans[cpt++] = gcnew array<int, 1>{ 60, 63, 66 };
			ans[cpt++] = gcnew array<int, 1>{ 64, 67 };
			ans[cpt++] = gcnew array<int, 1>{ 57, 65, 68 };

			// Retour
			return ans;

		}
		virtual array<bool, 2>^ calcGrid() override {
			
			// Tableau initial
			int h = nbLayers;
			int w = nbNotes;
			array<int, 2>^ initial = gcnew array<int, 2>(h, w);
			initial[h - 1, 0] = 1;

			// Sierpinskisation
			array<bool, 2>^ sierpinski = getSierpinski(initial, h, w, 11, true, false, false, gcnew array<int> { 1, 3, 5, 7, 9 });
			
			// Retourner
			return sierpinski;

		}
		virtual array<int, 1>^ getSierpinskiVerticalOffsets(int w) override {
			
			array<int, 1>^ ans = gcnew array<int, 1>(w);

			for (int i = 0; i < w; i++) ans[i] = 0;

			return ans;

		}
		virtual array<DynamicAndPosition^, 1>^ getGlobalDynamicsChanges(array<bool, 2>^ grid) override {

			// Créer la liste de dynamiques
			List<DynamicAndPosition^>^ ans = gcnew List<DynamicAndPosition^>;
			
			// Parties temporelles du morceau
			int part1_beg = 0;
			int part1_end = (int)Math::Round(Math::Pow(2, Math::Floor(Math::Round(Math::Log(nbLayers, 2), 9)))) - 1;
			int part2_beg = part1_end + 1;
			int part2_end = nbNotes - 1;
			
			// Remplir la liste de dynamiques
			ans->Add(gcnew DynamicAndPosition("!mp", 0));
			
			// Retourner sous forme de tableau
			return ans->ToArray();
			
		}
		virtual String^ getPlayingMode() override {
			
			return "pizz. (près du chevalet)";

		}
		virtual String^ getComments() override {
			
			return
				"Signature C" + "\r\n" +
				"Tempo noire = 100" + "\r\n" +
				"Echanges de parties dans les v1 vers mesure 9 pour que le bloc aigu se déplace du fond vers l'avant";
		}

};
public ref class G_APSO_Cloud : public Generator {

	// Types
	private:
		ref class CustomPattern : public CloudMaker::Pattern {
			
			private:
				int  _nbOctaves;
				bool _descending;
				
			public:
				CustomPattern(String^ elements_stringToParse, CloudMaker::PatternPossiblePosition^ possiblePosition, int nbOctaves, bool descending) :
					CloudMaker::Pattern(getElements(elements_stringToParse), possiblePosition),
					_nbOctaves (nbOctaves ),
					_descending(descending)
				{ }

			public:
				property int nbOctaves {
					int get() {
						return _nbOctaves;
					}
				}
				property bool descending {
					bool get() {
						return _descending;
					}
				}

			private:
				static array<Element^, 1>^ getElements(String^ elements_stringToParse) {
					
					Group^ group = qGroup(elements_stringToParse);

					return group->elements;

				}

		};

	// Options
	private:
		static const int nbBeats = 14;
		static const int dur_1024th = 256 * nbBeats;

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "APSO : Nuage pentatonique";

		}
		virtual String^ description() override {
			
			return "Nuage de motifs pentatoniques sur les cinq échelles contenant un fa, et dessinant une croissance vers le bas.";

		}
		virtual ScoreAndInfos^ generate() override {

			Rand::randomize(5);

			// Créer le nuage à l'aide d'un CloudMaker
			array<CloudMaker::PatternWithPos^, 1>^ cloud = getCloud();

			// Changer les notes
			{
				int nbPatterns = cloud->Length;
				for (int i = 0; i < nbPatterns; i++) {
					CloudMaker::PatternWithPos^ patternAndPos = cloud[i];
					CustomPattern^ pattern = (CustomPattern^)(cloud[i]->pattern);
					int pos_1024th = cloud[i]->pos_1024th;
					cloud[i] = gcnew CloudMaker::PatternWithPos(changeNotes(pattern, pos_1024th), pos_1024th);
				}
			}

			// Créer un bloc représentant ce nuage
			Block^ block = CloudMaker::CloudMaker::cloudToBlock(cloud);

			// Créer une représentation graphique
				{ /* PROV : le faire ! */ }

			// Retourner
			return gcnew ScoreAndInfos(block);

		}

	// Fonctions
	private:
		array<CloudMaker::PatternWithPos^, 1>^ getCloud() {
			
			// Créer le CloudMaker
			CloudMaker::CloudMaker^ cloudMaker = gcnew CloudMaker::CloudMaker(4 * nbBeats, RhythmFigure::_16th, gcnew TimeVariable_Exp(1.4, 16));

			// Ajouter les motifs possibles
			addPatterns_ascending (cloudMaker);
			addPatterns_descending(cloudMaker);

			// Retourner
			return cloudMaker->createCloud();

		}
		void addPatterns_ascending(CloudMaker::CloudMaker^ cloudMaker) {

			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"16th(|C4 16th|C4 16th)'|C4", // notes
					CloudMaker::PatternPossiblePosition::onlyOn16th, // positionnement
					1, // octaves
					false // ascendant
				),
				gcnew TimeVariable_Lin(2, 1) // probabilité au cours du temps
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"16th(|C4 16th|C4 16th|C4 16th)'|C4", // notes
					CloudMaker::PatternPossiblePosition::onlyOn16th, // positionnement
					1, // octaves
					false // ascendant
				),
				gcnew TimeVariable_Lin(2, 1.2) // probabilité au cours du temps
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"16th(|C4 16th|C4 16th|C4 16th|C4 16th)'|C4", // notes
					CloudMaker::PatternPossiblePosition::onlyOn16th, // positionnement
					1, // octaves
					false // ascendant
				),
				gcnew TimeVariable_Lin(1.5, 1.5) // probabilité au cours du temps
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"[16th:3:2 16th(|C4 16th|C4 16th)'|C4]", // notes
					CloudMaker::PatternPossiblePosition::onlyOnEighth, // positionnement
					1, // octaves
					false // ascendant
				),
				gcnew TimeVariable_Lin(2, 0) // probabilité au cours du temps
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"16th 16th(|C4 [16th:3:2 16th|C4 16th|C4 16th)'|C4]", // notes
					CloudMaker::PatternPossiblePosition::onlyOnEighth, // positionnement
					1, // octaves
					false // ascendant
				),
				gcnew TimeVariable_Lin(1.5, 1.5) // probabilité au cours du temps
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"[16th:3:2 16th 16th(|C4 16th|C4] [16th:3:2 16th|C4 16th|C4 16th)'|C4]", // notes
					CloudMaker::PatternPossiblePosition::onlyOnEighth, // positionnement
					1, // octaves
					false // ascendant
				),
				gcnew TimeVariable_Lin(1, 2) // probabilité au cours du temps
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"[16th:5:4 16th(|C4 16th|C4 16th|C4 16th|C4 16th)'|C4]", // notes
					CloudMaker::PatternPossiblePosition::onlyOnQuarter, // positionnement
					1, // octaves
					false // ascendant
				),
				gcnew TimeVariable_Lin(0, 2) // probabilité au cours du temps
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"16th(|C4 16th|C4 16th|C4 16th|C4 16th)'|C4", // notes
					CloudMaker::PatternPossiblePosition::onlyOn16th, // positionnement
					2, // octaves
					false // ascendant
				),
				gcnew TimeVariable_Exp(0.1, 2) // probabilité au cours du temps
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"16th(|C4 16th|C4 16th|C4 16th|C4 16th|C4 16th)'|C4", // notes
					CloudMaker::PatternPossiblePosition::onlyOnEighth, // positionnement
					2, // octaves
					false // ascendant
				),
				gcnew TimeVariable_Exp(0.1, 2) // probabilité au cours du temps
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"[16th:5:4 16th(|C4 16th|C4 16th|C4 16th|C4 16th)'|C4]", // notes
					CloudMaker::PatternPossiblePosition::onlyOnQuarter, // positionnement
					2, // octaves
					false // ascendant
				),
				gcnew TimeVariable_Exp(0.05, 1) // probabilité au cours du temps
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"[16th:3:2 16th(|C4 16th|C4 16th|C4] e)'|C4", // notes
					CloudMaker::PatternPossiblePosition::onlyOnEighth, // positionnement
					2, // octaves
					false // ascendant
				),
				gcnew TimeVariable_Exp(0.1, 3) // probabilité au cours du temps
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"16th(|C4 16th|C4 16th|C4 16th)'|C4", // notes
					CloudMaker::PatternPossiblePosition::onlyOn16th, // positionnement
					2, // octaves
					false // ascendant
				),
				gcnew TimeVariable_Exp(0.05, 3) // probabilité au cours du temps
			));

		}
		void addPatterns_descending(CloudMaker::CloudMaker^ cloudMaker) {

			TimeVariable^ proba = gcnew TimeVariable_Exp(0.5, 9);

			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"[e:3:2 e-|C4 e-|C4 e-|C4]", // notes
					CloudMaker::PatternPossiblePosition::onlyOnQuarter, // positionnement
					1, // octaves
					true // descendant
				),
				proba // probabilité au cours du temps
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"[e:3:2 e e-|C4 e-|C4] [e:3:2 e-|C4 e e]", // notes
					CloudMaker::PatternPossiblePosition::onlyOnQuarter, // positionnement
					1, // octaves
					true // descendant
				),
				proba // probabilité au cours du temps
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"[e:3:2 e e e-|C4] [e:3:2 e-|C4 e-|C4 e-|C4]", // notes
					CloudMaker::PatternPossiblePosition::onlyOnQuarter, // positionnement
					2, // octaves
					true // descendant
				),
				proba // probabilité au cours du temps
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CustomPattern(
					"[e:3:2 e-|C4 e-|C4 e-|C4] [e:3:2 e-|C4 e-|C4 e]", // notes
					CloudMaker::PatternPossiblePosition::onlyOnQuarter, // positionnement
					2, // octaves
					true // descendant
				),
				proba // probabilité au cours du temps
			));

		}
	private:
		ref struct ModeNote {
		
			public:
				int noteIndex;
				int octave;

			public:
				ModeNote(int noteIndex, int octave) :
					noteIndex(noteIndex),
					octave   (octave   )
				{ }
				ModeNote(int noteIndex) :
					noteIndex(noteIndex),
					octave   (0        )
				{ }
		
		};
		CloudMaker::Pattern^ changeNotes(CustomPattern^ oldPattern, int pos_1024th) {
			
			// Récupérer toutes les informations utiles pour établir les nouvelles notes
			int nbNotes;
			double time01;
			int nbOctaves;
			bool descending;
			{
				nbNotes = 0; {
					for each (Element^ element in oldPattern->elements) {
						if (element->isNote()) {
							nbNotes++;
						} else if (element->isTuplet()) {
							for each (Element^ subElement in ((Tuplet^)element)->elements) {
								if (subElement->isNote()) {
									nbNotes++;
								}
							}
						}
					}
				}
				time01 = ((double)pos_1024th) / (dur_1024th - 1);
				nbOctaves = oldPattern->nbOctaves;
				descending = oldPattern->descending;
			}

			// Choisir un mode pentatonique
			array<int, 1>^ mode = chooseMode(time01);

			// Décider d'une suite de 'nbNotes' notes dans le mode (repérées comme le numéro de note et son octave)
			array<ModeNote^, 1>^ notesInMode = chooseFigure(nbNotes, nbOctaves, descending, time01);

			// Parfois transposer d'un cran vers le bas
			if (Rand::double01() < 0.5) {
				for (int i = 0; i < nbNotes; i++) {
					ModeNote^ modeNote = notesInMode[i];
					int noteIndex = modeNote->noteIndex;
					int octave    = modeNote->octave   ;
					if (noteIndex > 0) {
						noteIndex--;
					} else {
						noteIndex = 4;
						octave--;
					}
					modeNote = gcnew ModeNote(noteIndex, octave);
					notesInMode[i] = modeNote;
				}
			}

			// Récupérer les notes MIDI correspondantes
			array<int, 1>^ pitches = gcnew array<int, 1>(nbNotes); {
				for (int i = 0; i < nbNotes; i++) {
					ModeNote^ modeNote = notesInMode[i];
					pitches[i] = mode[modeNote->noteIndex] + 12 * modeNote->octave;
				}
			}

			// Transposer la figure à la bonne octave
			int transp = chooseOctaveTransp(time01);
			for (int i = 0; i < nbNotes; i++) pitches[i] += transp;

			// Retourner une version de 'oldPattern' où les notes ont été remplacées par celles de 'figure'
			return changePitches(oldPattern, pitches);

		}
		static array<int, 1>^ chooseMode /* cinq valeurs MIDI (le fa 6 étant la dernière) */ (double time01) {
			
			// Choisir quel mode est à utiliser
			int modeIndex; {
				double time = time01 * 25;
				double random = Rand::double01();
				if (time <= 7) {
					// Plage 1
					if      (random < 1. / 3) modeIndex = 0;
					else                      modeIndex = 1;
				} else if (time <= 13) {
					// Plage 2
					if      (random < 1. / 4) modeIndex = 0;
					else if (random < 2. / 4) modeIndex = 1;
					else                      modeIndex = 2;
				} else if (time <= 18) {
					// Plage 3
					if      (random < 1. / 5) modeIndex = 0;
					else if (random < 2. / 5) modeIndex = 1;
					else if (random < 3. / 5) modeIndex = 2;
					else                      modeIndex = 3;
				} else if (time <= 22) {
					// Plage 4
					if      (random < 1. / 6) modeIndex = 0;
					else if (random < 2. / 6) modeIndex = 1;
					else if (random < 3. / 6) modeIndex = 2;
					else if (random < 4. / 6) modeIndex = 3;
					else                      modeIndex = 4;
				} else {
					// Plage 5
					if      (random < 1. / 5) modeIndex = 0;
					else if (random < 2. / 5) modeIndex = 1;
					else if (random < 3. / 5) modeIndex = 2;
					else if (random < 4. / 5) modeIndex = 3;
					else                      modeIndex = 4;
				}
			}

			// Retourner les notes du mode
			if      (modeIndex == 0) return gcnew array<int, 1> { 79, 81, 84, 86, 89 };
			else if (modeIndex == 1) return gcnew array<int, 1> { 79, 82, 84, 86, 89 };
			else if (modeIndex == 2) return gcnew array<int, 1> { 79, 82, 84, 87, 89 };
			else if (modeIndex == 3) return gcnew array<int, 1> { 80, 82, 84, 87, 89 };
			else                     return gcnew array<int, 1> { 80, 82, 85, 87, 89 };

		}
		static array<ModeNote^, 1>^ chooseFigure /* commencer ou terminer sur la dernière note du mode */ (int nbNotes, int nbOctaves, bool descending, double time01) {

			// Cas triviaux
			if      (nbNotes == 0) return gcnew array<ModeNote^, 1> { };
			else if (nbNotes == 1) return gcnew array<ModeNote^, 1> { gcnew ModeNote(4) };
			
			// Motif ascendant tenant dans une octave
			if (!descending && nbOctaves == 1) {
				if (nbNotes == 2) {
					return Rand::chooseIn(gcnew array<array<ModeNote^, 1>^> {
						gcnew array<ModeNote^, 1> { gcnew ModeNote(0), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(1), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(2), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(3), gcnew ModeNote(4) }
					});
				} else if (nbNotes == 3) {
					return Rand::chooseIn(gcnew array<array<ModeNote^, 1>^> {
						gcnew array<ModeNote^, 1> { gcnew ModeNote(0), gcnew ModeNote(1), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(0), gcnew ModeNote(2), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(0), gcnew ModeNote(3), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(1), gcnew ModeNote(2), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(1), gcnew ModeNote(3), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(2), gcnew ModeNote(3), gcnew ModeNote(4) }
					});
				} else if (nbNotes == 4) {
					return Rand::chooseIn(gcnew array<array<ModeNote^, 1>^> {
						gcnew array<ModeNote^, 1> { gcnew ModeNote(0), gcnew ModeNote(1), gcnew ModeNote(2), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(0), gcnew ModeNote(1), gcnew ModeNote(3), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(0), gcnew ModeNote(2), gcnew ModeNote(3), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(1), gcnew ModeNote(2), gcnew ModeNote(3), gcnew ModeNote(4) }
					});
				} else if (nbNotes == 5) {
					return gcnew array<ModeNote^, 1> { gcnew ModeNote(0), gcnew ModeNote(1), gcnew ModeNote(2), gcnew ModeNote(3), gcnew ModeNote(4) };
				}
			}
			
			// Motif descendant tenant dans une octave
			if (descending && nbOctaves == 1) {
				if (nbNotes == 2) {
					return Rand::chooseIn(gcnew array<array<ModeNote^, 1>^> {
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(0)  },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(1)  },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(2)  },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(3)  }
					});
				} else if (nbNotes == 3) {
					return Rand::chooseIn(gcnew array<array<ModeNote^, 1>^> {
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(1), gcnew ModeNote(0) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(2), gcnew ModeNote(0) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(3), gcnew ModeNote(0) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(2), gcnew ModeNote(1) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(3), gcnew ModeNote(1) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(3), gcnew ModeNote(2) }
					});
				} else if (nbNotes == 4) {
					return Rand::chooseIn(gcnew array<array<ModeNote^, 1>^> {
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(2), gcnew ModeNote(1), gcnew ModeNote(0) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(3), gcnew ModeNote(1), gcnew ModeNote(0) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(3), gcnew ModeNote(2), gcnew ModeNote(0) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(3), gcnew ModeNote(2), gcnew ModeNote(1) }
					});
				} else if (nbNotes == 5) {
					return gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(3), gcnew ModeNote(2), gcnew ModeNote(1), gcnew ModeNote(0) };
				}
			}

			// Motif ascendant tenant dans deux octaves
			if (!descending && nbOctaves == 2) {
				if (nbNotes == 2) {
					return Rand::chooseIn(gcnew array<array<ModeNote^, 1>^> {
						gcnew array<ModeNote^, 1> { gcnew ModeNote(2, -1), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(3, -1), gcnew ModeNote(4) }
					});
				} else if (nbNotes == 3) {
					return Rand::chooseIn(gcnew array<array<ModeNote^, 1>^> {
						gcnew array<ModeNote^, 1> { gcnew ModeNote(1, -1), gcnew ModeNote(3, -1), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(1, -1), gcnew ModeNote(0    ), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(1, -1), gcnew ModeNote(1    ), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(2, -1), gcnew ModeNote(0    ), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(2, -1), gcnew ModeNote(1    ), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(2, -1), gcnew ModeNote(3    ), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(3, -1), gcnew ModeNote(0    ), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(3, -1), gcnew ModeNote(1    ), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(3, -1), gcnew ModeNote(2    ), gcnew ModeNote(4) }
					});
				} else if (nbNotes == 4) {
					return Rand::chooseIn(gcnew array<array<ModeNote^, 1>^> {
						gcnew array<ModeNote^, 1> { gcnew ModeNote(3, -1), gcnew ModeNote(0), gcnew ModeNote(2), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(1, -1), gcnew ModeNote(0), gcnew ModeNote(2), gcnew ModeNote(4) }
					});
				} else if (nbNotes == 5) {
					return Rand::chooseIn(gcnew array<array<ModeNote^, 1>^> {
						gcnew array<ModeNote^, 1> { gcnew ModeNote(3, -1), gcnew ModeNote(0    ), gcnew ModeNote(1), gcnew ModeNote(2), gcnew ModeNote(4) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(1, -1), gcnew ModeNote(3, -1), gcnew ModeNote(0), gcnew ModeNote(2), gcnew ModeNote(4) },
					});
				} else if (nbNotes == 6) {
					return gcnew array<ModeNote^, 1> { gcnew ModeNote(2, -1), gcnew ModeNote(3, -1), gcnew ModeNote(0), gcnew ModeNote(1), gcnew ModeNote(3), gcnew ModeNote(4) };
				}
			}

			// Motif descendant tenant dans deux octaves
			if (descending && nbOctaves == 2) {
				if (nbNotes == 2) {
					return Rand::chooseIn(gcnew array<array<ModeNote^, 1>^> {
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(2, -1) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(3, -1) }
					});
				} else if (nbNotes == 3) {
					return Rand::chooseIn(gcnew array<array<ModeNote^, 1>^> {
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(3, -1), gcnew ModeNote(1, -1) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(0    ), gcnew ModeNote(1, -1) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(1    ), gcnew ModeNote(1, -1) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(0    ), gcnew ModeNote(2, -1) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(1    ), gcnew ModeNote(2, -1) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(3    ), gcnew ModeNote(2, -1) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(0    ), gcnew ModeNote(3, -1) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(1    ), gcnew ModeNote(3, -1) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(2    ), gcnew ModeNote(3, -1) }
					});
				} else if (nbNotes == 4) {
					return Rand::chooseIn(gcnew array<array<ModeNote^, 1>^> {
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(2), gcnew ModeNote(0), gcnew ModeNote(3, -1) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(2), gcnew ModeNote(0), gcnew ModeNote(1, -1) }
					});
				} else if (nbNotes == 5) {
					return Rand::chooseIn(gcnew array<array<ModeNote^, 1>^> {
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(2), gcnew ModeNote(1), gcnew ModeNote(0    ), gcnew ModeNote(3, -1) },
						gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(2), gcnew ModeNote(0), gcnew ModeNote(3, -1), gcnew ModeNote(1, -1) },
					});
				} else if (nbNotes == 6) {
					return gcnew array<ModeNote^, 1> { gcnew ModeNote(4), gcnew ModeNote(3), gcnew ModeNote(1), gcnew ModeNote(0), gcnew ModeNote(3, -1), gcnew ModeNote(2, -1) };
				}
			}

			// Erreur si on est arrivé ici
			throw gcnew Exception;

		}
		static int chooseOctaveTransp(double time01) {
			
			// Déterminer les octaves possibles
			int minTransp_octaves, maxTransp_octaves; {
				double time = time01 * 26;
				if (time < 8) {
					minTransp_octaves = -1;
					maxTransp_octaves =  0;
				} else if (time < 15) {
					minTransp_octaves = -2;
					maxTransp_octaves =  0;
				} else if (time < 21) {
					minTransp_octaves = -3;
					maxTransp_octaves =  0;
				} else {
					minTransp_octaves = -3;
					maxTransp_octaves = +1;
				}
			}

			// Choisir l'octave, en donnant du privilège aux plus graves (selon le moment)
			int octave; {
				if (minTransp_octaves == maxTransp_octaves) {
					octave = minTransp_octaves;
				} else {
					double rand = applyLowOctavePrivilege(Rand::double01(), time01);
					double inRange = minTransp_octaves + rand * (maxTransp_octaves - minTransp_octaves + 0.99999) - 0.5;
					octave = (int)Math::Round(inRange);
					if (octave < minTransp_octaves || octave > maxTransp_octaves) throw gcnew Exception;
				}
			}

			// Retourner
			return 12 * octave;

		}
		static double applyLowOctavePrivilege(double rand, double time01) {
			
			// Déterminer le privilège des graves (0 pour que des graves, 0.5 pour normal, 1 pour que des aigus)
			double privilege; {
				double whenChanges = 0.85;
				double lowestValue = 0.12;
				if (time01 < whenChanges) {
					// Première section : le privilège va linéairement de 0.5 à 'lowestValue'
					double time_0_1_inSection = time01 / whenChanges;
					privilege = (gcnew TimeVariable_Lin(0.5, lowestValue))->at(time_0_1_inSection);
				} else {
					// Seconde section : le privilège va linéairement de 'lowestValue' à 1
					double time_0_1_inSection = (time01 - whenChanges) / (1 - whenChanges);
					privilege = (gcnew TimeVariable_Lin(lowestValue, 1))->at(time_0_1_inSection);
				}
			}

			// Plier 'rand'
			if (privilege == 0) {
				return 0;
			} else if (privilege == 1) {
				return 1;
			} else {
				// Déterminer le point de pli de 'rand'
				double x = 1 - privilege;
				double y =     privilege;
				// Plier
				if (rand < x) return      rand      *      y  /      x ;
				else          return y + (rand - x) * (1 - y) / (1 - x);
			}

		}
		static CloudMaker::Pattern^ changePitches(CloudMaker::Pattern^ pattern, array<int, 1>^ pitches) {
			
			// Extraire les éléments
			array<Element^, 1>^ elements = pattern->elements;
			int nbElements = elements->Length;

			// Changer le pitch de chaque élément
			array<Element^, 1>^ newElements = gcnew array<Element^, 1>(nbElements);
			int cpt_pitches = 0;
			for (int i = 0; i < nbElements; i++) {
				Element^ element = elements[i];
				if (element->isRest()) {
					newElements[i] = element;
				} else if (element->isNote()) {
					Pitch^ pitch = gcnew Pitch(pitches[cpt_pitches++]);
					newElements[i] = gcnew Note((Note^)element, pitch);
				} else if (element->isTuplet()) {
					// Extraire les éléments
					array<NoteOrRest^, 1>^ subElements = ((Tuplet^)element)->elements;
					int nbSubElements = subElements->Length;
					// Changer le pitch de chaque élément
					array<NoteOrRest^, 1>^ newSubElements = gcnew array<NoteOrRest^, 1>(nbSubElements);
					for (int j = 0; j < nbSubElements; j++) {
						NoteOrRest^ subElement = subElements[j];
						if (subElement->isRest()) {
							newSubElements[j] = subElement;
						} else if (subElement->isNote()) {
							Pitch^ pitch = gcnew Pitch(pitches[cpt_pitches++]);
							newSubElements[j] = gcnew Note((Note^)subElement, pitch);
						} else if (subElement->isTuplet()) {
							throw gcnew Exception;
						} else {
							throw gcnew Exception;
						}
					}
					// Ajouter
					newElements[i] = gcnew Tuplet(((Tuplet^)element)->ratio, newSubElements);
				} else {
					throw gcnew Exception;
				}
			}

			// Retourner
			return gcnew CloudMaker::Pattern(newElements, pattern->possiblePosition);

		}

};
public ref class G_APSO_Sculpture : public Generator {

	// Options
	private:
		static const int nbParts = 29;

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "APSO : Sculpture";

		}
		virtual String^ description() override {
			
			return "Nuage de notes isolées fixées sur un mode, et dessinant un forme personnalisée.";

		}
		virtual ScoreAndInfos^ generate() override {

			Rand::randomize(0);

			// Créer le mode à 'nbParts' notes
			array<int, 1>^ mode = createMode();

			// Créer le tableau des probabilités de jeu (pour une partie donnée, à un temps donné) --> 0 ou 1 pour l'instant
			array<double, 2>^ probas = createProbas();
			int nbBeats = probas->GetUpperBound(1) + 1;

			// Créer le tableau des jeux effectifs (pour une partie donnée, à un temps donné) --> 
			array<bool, 2>^ play = gcnew array<bool, 2>(nbParts, nbBeats); {
				for (int part = 0; part < nbParts; part++) {
					for (int beat = 0; beat < nbBeats; beat++) {
						play[part, beat] = Rand::double01() < probas[part, beat];
					}
				}
			}

			// Créer un bloc avec une mesure unique
			Block^ block = gcnew Block(nbParts);
			block->addBar(nbBeats + "/4");

			// Ajouter les éléments de chaque partie
			for (int part = 1; part <= nbParts; part++) {
				List<Element^>^ partContents = gcnew List<Element^>;
				int partPitch = mode[nbParts - part];
				for (int beat = 0; beat < nbBeats; beat++) {
					bool oneBeat = play[part - 1, beat];
					bool twoConsecutiveBeats = (beat < nbBeats - 1) && oneBeat && play[part - 1, beat + 1] && (Rand::double01() < 0.5);
					// Ajouter des éléments sur un ou deux temps, ou un silence
					if      (twoConsecutiveBeats) { addRandomBeatContent(partContents, partPitch, 2); beat++; }
					else if (oneBeat)             { addRandomBeatContent(partContents, partPitch, 1);         }
					else                          { partContents->Add(qElement("q"));				           }
				}
				block[part, 1] = gcnew Group(partContents);
			}

			// Créer une représentation graphique


			// Retourner
			return block;

		}

	// Fonctions
	private:
		static array<int, 1>^ createMode() {
			
			array<int, 1>^ ans = gcnew array<int, 1>(nbParts);

			// CloudMaker::Pattern de base : 4 notes chromatiques
			int patternSize = 4;
			int cpt = 0;
			ans[cpt++] = 37;
			ans[cpt++] = ans[cpt - 1] + 1;
			ans[cpt++] = ans[cpt - 1] + 1;
			ans[cpt++] = ans[cpt - 1] + 1;

			// Répétition du pattern à une quinte d'écart
			int patternsOffset = 7;
			for (int i = patternSize; i < nbParts; i++) ans[i] = ans[i - patternSize] + patternsOffset;

			// Retourner
			return ans;

		}
	private:
		static array<double, 2>^ createProbas() {
			
			List<array<double, 1>^>^ ans_lst = gcnew List<array<double, 1>^>;

			// Ajouter chaque section
			addProbasSections(ans_lst);

			// Convertir en tableau bi-dimensionnel
			int nbBeats = ans_lst->Count;
			array<double, 2>^ ans = gcnew array<double, 2>(nbParts, nbBeats);
			int beat = 0;
			for each (array<double, 1>^ arrayAtBeat in ans_lst) {
				for (int part = 0; part < nbParts; part++) {
					ans[part, beat] = arrayAtBeat[part];
				}
				beat++;
			}

			// Retourner
			return ans;

		}
		static void addProbasSections(List<array<double, 1>^>^ list_out) {
			
			addProbasSection(list_out, createProbas_section1(10));
			addProbasSection(list_out, createProbas_section2(13));
			addProbasSection(list_out, createProbas_section3( 3));
			addProbasSection(list_out, createProbas_section4(12));
			addProbasSection(list_out, createProbas_section5(10));
			addProbasSection(list_out, createProbas_section6( 5));
			addProbasSection(list_out, createProbas_section7(12));

		}
		static void addProbasSection(List<array<double, 1>^>^ list_out, array<double, 2>^ section) {
			
			int nbBeatsToAdd = section->GetUpperBound(1) + 1;

			for (int beat = 0; beat < nbBeatsToAdd; beat++) {
				array<double, 1>^ arrayToAdd = gcnew array<double, 1>(nbParts);
				for (int part = 0; part < nbParts; part++) {
					arrayToAdd[part] = section[part, beat];
				}
				list_out->Add(arrayToAdd);
			}

		}
		static array<double, 2>^ createProbas_section1(int nbBeats) {
			
			return createProbasFromRangesInTime(nbBeats, gcnew array<TimeVariable^, 1> {
				gcnew TimeVariable_Const(0),
				gcnew TimeVariable_Const(1)
			});

		}
		static array<double, 2>^ createProbas_section2(int nbBeats) {
			
			return createProbasFromRangesInTime(nbBeats, gcnew array<TimeVariable^, 1> {
				gcnew TimeVariable_Lin(0, 0.7),
				gcnew TimeVariable_Lin(1, 0.8)
			});

		}
		static array<double, 2>^ createProbas_section3(int nbBeats) {
			
			return createProbasFromRangesInTime(nbBeats, gcnew array<TimeVariable^, 1> {
				gcnew TimeVariable_Const(0.7),
				gcnew TimeVariable_Const(0.8)
			});

		}
		static array<double, 2>^ createProbas_section4(int nbBeats) {
			
			return createProbasFromRangesInTime(nbBeats, gcnew array<TimeVariable^, 1> {
				gcnew TimeVariable_Lin(0.3, 0.35),
				gcnew TimeVariable_Lin(0.3, 0.5),
				gcnew TimeVariable_Const(0.7),
				gcnew TimeVariable_Const(0.8)
			});

		}
		static array<double, 2>^ createProbas_section5(int nbBeats) {
			
			return createProbasFromRangesInTime(nbBeats, gcnew array<TimeVariable^, 1> {
				gcnew TimeVariable_Lin(0.35, 0.4),
				gcnew TimeVariable_Lin(0.5, 0.6),
				gcnew TimeVariable_Lin(0.7, 0.6),
				gcnew TimeVariable_Lin(0.8, 0.75)
			});

		}
		static array<double, 2>^ createProbas_section6(int nbBeats) {
			
			return createProbasFromRangesInTime(nbBeats, gcnew array<TimeVariable^, 1> {
				gcnew TimeVariable_Lin(0.4, 0.6),
				gcnew TimeVariable_Lin(0.75, 0.7)
			});

		}
		static array<double, 2>^ createProbas_section7(int nbBeats) {
			
			return createProbasFromRangesInTime(nbBeats, gcnew array<TimeVariable^, 1> {
				gcnew TimeVariable_Const(0.6),
				gcnew TimeVariable_Const(0.7)
			});

		}
		static array<double, 2>^ createProbasFromRangesInTime(int nbBeats, array<TimeVariable^, 1>^ bounds01) {
			
			array<double, 2>^ ans = gcnew array<double, 2>(nbParts, nbBeats);
			int nbBounds = bounds01->Length; if (nbBounds % 2 != 0) throw gcnew Exception;

			// Parcourir les temps
			for (int beat = 0; beat < nbBeats; beat++) {
				// Récupérer les bornes à ce point temporel, transposées dans (0, nbParts - 1)
				array<double, 1>^ bounds_atThisBeat = gcnew array<double, 1>(nbBounds);
				for (int i = 0; i < nbBounds; i++) bounds_atThisBeat[i] = (nbParts - 1) * bounds01[i]->at(beat, nbBeats);
				// Parcourir les parties
				for (int part = 0; part < nbParts; part++) {
					// Dans les bornes ?
					bool isInBounds = false;
					for (int i = 0; i < nbBounds; i += 2) {
						int bound_lo = (int)Math::Round(bounds_atThisBeat[i    ]);
						int bound_hi = (int)Math::Round(bounds_atThisBeat[i + 1]);
						int part_reverse = nbParts - 1 - part;
						if (part_reverse >= bound_lo && part_reverse <= bound_hi) {
							isInBounds = true;
							break;
						}
					}
					// Enregistrer
					ans[part, beat] = isInBounds ? 1 : 0;
				}
			}

			// Retourner
			return ans;

		}
	private:
		static void addRandomBeatContent(List<Element^>^ elements_out, int midiPitch, int nbBeats) {
			
			String^ elementsToAdd;

			// Choisir le contenu
			if (nbBeats == 1) {
				elementsToAdd = Rand::chooseIn(gcnew array<String^, 1> {
					"e| e|",
					"e| e|",
					"e| e|",
					"e| e|",
					"e| e",
					"e e|",
					"16th e.|",
					"16th e.|",
					"16th e.|",
					"16th e.|"
				});
			} else if (nbBeats == 2) {
				elementsToAdd = Rand::chooseIn(gcnew array<String^, 1> {
					"[e:3:2 q e|] [e:3:2 e e| e]",
					"[e:3:2 q e|] [e:3:2 e e| e]",
					"e.| 16th| e e|"
				});
			} else throw gcnew Exception;

			// Ajouter l'articulation avant et le pitch après chaque '|'
			elementsToAdd = elementsToAdd->Replace("|", "'|" + midiPitch);

			// Ajouter les éléments
			Group^ group = qGroup(elementsToAdd);
			array<Element^, 1>^ elements = group->elements;
			for each (Element^ element in elements) {
				elements_out->Add(element);
			}

		}

};
public ref class G_APSO_Gliss : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "APSO : Glissades";

		}
		virtual String^ description() override {
			
			return "Nuage de glissades de cordes.";

		}
		virtual ScoreAndInfos^ generate() override {

			Rand::randomize(17 - 04 - 1991);

			// Créer le nuage organisé, manuellement (élément par élément)
			array<CloudMaker::PatternWithPos^, 1>^ cloud = getCloud();

			// Créer un bloc représentant ce nuage
			Block^ block = CloudMaker::CloudMaker::cloudToBlock(cloud);

			// Créer une représentation graphique
				{ /* PROV : le faire ! */ }

			// Retourner
			return gcnew ScoreAndInfos(block);

		}

	// Fonctions
	private:
		static array<CloudMaker::PatternWithPos^, 1>^ getCloud() {
			
			// Stocker les coordonnées de chaque chemin du dessin
			array<array<int, 1>^, 1>^ coordinates = gcnew array<array<int, 1>^, 1> {
				gcnew array<int, 1> { 0, 0, 0,  0, 10, 6, 3,  8},
				gcnew array<int, 1> { 0, 7, 0,  2, 11, 1, 3,  8},
				gcnew array<int, 1> { 1, 6, 0,  4, 11, 7, 3,  9},
				gcnew array<int, 1> { 2, 5, 0,  6, 11, 7, 3,  8},
				gcnew array<int, 1> { 3, 4, 0,  8, 12, 4, 3, 10},
				gcnew array<int, 1> { 4, 3, 0, 10, 12, 4, 3,  9},
				gcnew array<int, 1> { 5, 2, 1,  0, 12, 4, 3,  8},
				gcnew array<int, 1> { 6, 1, 1,  2, 12, 4, 3,  7},
				gcnew array<int, 1> { 7, 0, 1,  4, 13, 3, 3, 11},
				gcnew array<int, 1> { 7, 7, 1,  6, 13, 3, 3, 10},
				gcnew array<int, 1> { 8, 6, 1,  8, 13, 3, 3,  9},
				gcnew array<int, 1> { 3, 5, 0, -2, 13, 3, 3,  8},
				gcnew array<int, 1> { 4, 4, 0,  0, 14, 2, 4,  0},
				gcnew array<int, 1> { 5, 3, 0,  2, 14, 2, 3, 11},
				gcnew array<int, 1> { 6, 2, 0,  4, 14, 2, 3, 10},
				gcnew array<int, 1> { 7, 1, 0,  6, 14, 2, 3,  9},
				gcnew array<int, 1> { 8, 0, 0,  8, 15, 1, 4,  1},
				gcnew array<int, 1> { 8, 7, 0, 10, 15, 1, 4,  0},
				gcnew array<int, 1> { 9, 6, 1,  0, 15, 1, 3, 11},
				gcnew array<int, 1> {10, 5, 1,  2, 15, 1, 3, 10}
			};
			//array<array<int, 1>^, 1>^ coordinates = gcnew array<array<int, 1>^, 1> {
			//	gcnew array<int, 1> { 12, 6, 1, 12, 16, 0, 2, 26 },
			//	gcnew array<int, 1> { 12, 6, 1, 11, 16, 0, 2, 25 },
			//	gcnew array<int, 1> { 12, 6, 1, 10, 16, 0, 2, 24 },
			//	gcnew array<int, 1> { 12, 6, 1,  9, 16, 0, 2, 23 },
			//	gcnew array<int, 1> { 13, 5, 1, 10, 16, 0, 2, 17 },
			//	gcnew array<int, 1> { 13, 5, 1,  9, 16, 0, 2, 16 },
			//	gcnew array<int, 1> { 13, 5, 1,  8, 16, 0, 2, 15 },
			//	gcnew array<int, 1> { 13, 5, 1,  7, 16, 0, 2, 14 },
			//	gcnew array<int, 1> { 14, 4, 1,  8, 16, 0, 2,  8 },
			//	gcnew array<int, 1> { 14, 4, 1,  7, 16, 0, 2,  7 },
			//	gcnew array<int, 1> { 14, 4, 1,  6, 16, 0, 2,  6 },
			//	gcnew array<int, 1> { 14, 4, 1,  5, 16, 0, 2,  5 },
			//	gcnew array<int, 1> { 15, 3, 1,  6, 16, 0, 2, -1 },
			//	gcnew array<int, 1> { 15, 3, 1,  5, 16, 0, 2, -2 },
			//	gcnew array<int, 1> { 15, 3, 1,  4, 16, 0, 2, -3 },
			//	gcnew array<int, 1> { 15, 3, 1,  3, 16, 0, 2, -4 },
			//};

			// Calculer les patterns de chaque chemin et les ajouter à une liste unique
			List<CloudMaker::PatternWithPos^>^ ans = gcnew List<CloudMaker::PatternWithPos^>;
			int nbPathes = coordinates->Length;
			for each (array<int, 1>^ pathCoords in coordinates) {
				List<CloudMaker::PatternWithPos^>^ patterns = calcPatterns(pathCoords[0], pathCoords[1], pathCoords[2], pathCoords[3], pathCoords[4], pathCoords[5], pathCoords[6], pathCoords[7]);
				for each (CloudMaker::PatternWithPos^ pattern in patterns) {
					ans->Add(pattern);
				}
			}

			// Retourner
			return ans->ToArray();

		}
		static List<CloudMaker::PatternWithPos^>^ calcPatterns(int beg_bar, int beg_eighthInBar, int beg_octave, int beg_pitchInOctave, int end_bar, int end_eighthInBar, int end_octave, int end_pitchInOctave) {
			
			return calcPatterns(
				8 * beg_bar + beg_eighthInBar, 45 + 12 * beg_octave + beg_pitchInOctave,
				8 * end_bar + end_eighthInBar, 45 + 12 * end_octave + end_pitchInOctave
			);

		}
		static List<CloudMaker::PatternWithPos^>^ calcPatterns(int beg_eighth, int beg_pitch, int end_eighth, int end_pitch) {
		
			int nbEighths = end_eighth - beg_eighth + 1;
			List<CloudMaker::PatternWithPos^>^ ans = gcnew List<CloudMaker::PatternWithPos^>;

			// Etablir le pitch de chaque croche
			array<int, 1>^ pitches = gcnew array<int, 1>(nbEighths); {
				int pitch = beg_pitch;
				pitches[0] = beg_pitch;
				for (int i = 1; i < nbEighths; i++) {
					int eighth = beg_eighth + i;
					if (eighth <= 96) pitches[i] = pitches[i - 1] + ((i % 4 > 0) ? 1 : -1);
					else              pitches[i] = pitches[i - 1] + 1;
				}
				if (pitches[nbEighths - 1] != end_pitch) {
					throw gcnew Exception("Incorrect path.");
				}
			}

			// Etablir les points de coupure, toutes les 12 croches sauf exception
			array<int, 1>^ cuts;
			int nbCuts;
			{
				List<int>^ cuts_lst = gcnew List<int>;
				for (int i = 0; i < nbEighths; i += 12) {
					cuts_lst->Add(i);
				}
				if (cuts_lst[cuts_lst->Count - 1] != nbEighths - 1) cuts_lst->Add(nbEighths - 1);
				cuts = cuts_lst->ToArray();
				nbCuts = cuts->Length;
				// Eviter une dernière section trop courte
				if (nbCuts >= 3 && (cuts[nbCuts - 1] - cuts[nbCuts - 2] < 6)) {
					cuts[nbCuts - 2] -= 4;
				}
			}

			// Diviser en sections, tout en ajoutant les éléments
			for (int i = 0; i < nbCuts - 1; i++) {
				int cut1 = cuts[i    ];
				int cut2 = cuts[i + 1];
				int nbPitchesInThisCut = cut2 - cut1 + 1;
				array<int, 1>^ pitchesInThisCut = gcnew array<int, 1>(nbPitchesInThisCut);
				for (int j = 0; j < nbPitchesInThisCut; j++) pitchesInThisCut[j] = pitches[cut1 + j];
				ans->Add(calcPattern(pitchesInThisCut, (beg_eighth + cut1) * RhythmFigure::_eighth->duration_1024th));
			}

			// Retourner
			return ans;

		}
		static CloudMaker::PatternWithPos^ calcPattern(array<int, 1>^ pitches, int pos_1024th) {
			
			int nbEights = pitches->Length;
			List<Element^>^ elements = gcnew List<Element^>;
			
			array<bool, 1>^ keep = gcnew array<bool, 1>(nbEights);
			for (int i = 0; i < nbEights; i++) {
				keep[i] = (i == 0) || (i == nbEights - 1) || (2 * pitches[i] != pitches[i - 1] + pitches[i + 1]);
			}

			RhythmFigure^ eighth = RhythmFigure::_eighth;
			for (int i = 0; i < nbEights; i++) {
				if (keep[i]) {
					array<Pitch^, 1>^ pitchesInNote = gcnew array<Pitch^, 1> { gcnew Pitch(pitches[i]) };
					NoteDecorations^ deco;
					if      (i == 0           ) deco = gcnew NoteDecorations(false, true, false);
					else if (i <  nbEights - 1) deco = gcnew NoteDecorations();
					else                        deco = gcnew NoteDecorations(Articulation::staccato, false, false, true);
					elements->Add(gcnew Note(eighth, pitchesInNote, deco));
				} else {
					elements->Add(gcnew Rest(eighth));
				}
			}

			return gcnew CloudMaker::PatternWithPos(
				gcnew CloudMaker::Pattern(
					elements->ToArray(),
					CloudMaker::PatternPossiblePosition::unspecified
				),
				pos_1024th
			);

		}

};
public ref class G_APSO_Combinations : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "APSO : Combinaisons";

		}
		virtual String^ description() override {
			
			return "Toutes les combinaisons sur un certain nombre de notes.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			// Récupérer les options
			int p;
			int n;
			bool alsoSmallerCombinations;
			array<String^, 1>^ mode;
			RhythmFigure^ baseRhythm;
			int nbBaseRhythmsPerBar;
			int combinationsSorting;
			char figuresType;
			bool slurs;
			{
				// Montrer la boîte de dialogue
				ScoreGen::OF_Combinations^ dialog = gcnew ScoreGen::OF_Combinations(name(), description());
				dialog->ShowDialog();
				// Récupérer les valeurs des options
				array<Object^, 1>^ options = dialog->getOptions();
				if (options == nullptr) return nullptr;
				// Convertir
				mode                    =                    (array<String^, 1>^)(options[0]);
				n                       =                    (int               )(options[1]);
				p                       =                    (int               )(options[2]);
				baseRhythm              = gcnew RhythmFigure((int               )(options[3]));
				nbBaseRhythmsPerBar     =                    (int               )(options[4]);
				alsoSmallerCombinations =                    (bool              )(options[5]);
				combinationsSorting     =                    (int               )(options[6]);
				figuresType             =                    (char              )(options[7]);
				slurs                   =                    (bool              )(options[8]);
			}
			
			// Vérifier que le mode a la bonne taille par rapport à 'n'
			if (mode->Length != n) throw gcnew Exception("The mode size doesn't match the value of 'n'.");
			
			// Faire la liste des combinaisons (p parmi n, et éventuellement plus petites) : groupé par le bas --> groupé par le haut
			array<array<int, 1>^, 1>^ combinations = getCombinations_withSmallerIfNecessary(p, n, alsoSmallerCombinations, combinationsSorting)->ToArray();

			// Transformer le mode : String^ --> Pitch^
			array<Pitch^, 1>^ pitches = gcnew array<Pitch^, 1>(n); {
				try {
					for (int i = 0; i < n; i++) {
						pitches[i] = qPitch(mode[i]);
					}
				} catch (...) {
					throw gcnew Exception("The mode is invalid.");
				}
			}

			// Modifier pour coller au mode demandé
			array<array<Pitch^, 1>^, 1>^ notesGroups = getNotesGroupsInMode(combinations, pitches);

			// Transformer en une liste de motifs musicaux représentant ces combinaisons (accords ou groupes de notes successives)
			array<array<Element^, 1>^, 1>^ musicalGroups = getMusicalGroups(notesGroups, baseRhythm, figuresType, slurs);

			// Dérouler la liste d'éléments
			List<Element^>^ elements = gcnew List<Element^>; for each (array<Element^, 1>^ figure in musicalGroups) for each (Element^ element in figure) elements->Add(element);

			// Retourner le block
			return createBlock(elements->ToArray(), baseRhythm, nbBaseRhythmsPerBar);
			
		}

	// Fonctions
	private:
		static List<array<int, 1>^>^ getCombinations_withSmallerIfNecessary(int pMax, int n, bool alsoSmallerCombinations, int combinationsSorting) {
			
			List<array<int, 1>^>^ ans = gcnew List<array<int, 1>^>;

			// Ajouter pour chaque 'p' demandé
			int init_p = alsoSmallerCombinations ? 1 : pMax;
			for (int p = pMax; p >= init_p; p--) {
				List<array<int, 1>^>^ combinations_p = getCombinations(p, n, combinationsSorting);
				for each (array<int, 1>^ combination in combinations_p) {
					ans->Add(combination);
				}
			}

			// Retourner
			return ans;

		}
		static List<array<int, 1>^>^ getCombinations(int p, int n, int combinationsSorting) {
			
			if      (combinationsSorting == 1) return getCombinations_sorting1(p, n);
			else if (combinationsSorting == 2) return getCombinations_sorting2(p, n);
			else if (combinationsSorting == 3) return getCombinations_sorting3(p, n);
			else                               throw gcnew Exception;

		}
		ref class Comparer_sorting2 : public Generic::Comparer<array<int, 1>^> {
			
			public:
				virtual int Compare(array<int, 1>^ combination1, array<int, 1>^ combination2) override  {
					
					int len = combination1->Length;
					int len2 = combination2->Length;
					if (len != len2) throw gcnew Exception("To compare two combinations, they should have the same length.");
					int val1, val2;

					for (int i = len - 1; i >= 0; i--) {
						val1 = combination1[i];
						val2 = combination2[i];
						if      (val1 < val2) return -1;
						else if (val1 > val2) return  1;
					}

					return 0;

				}

		};
		static List<array<int, 1>^>^ getCombinations_sorting1(int p, int n) {
			
			List<array<int, 1>^>^ ans = gcnew List<array<int, 1>^>;

			// Créer et ajouter la première combinaison
			array<int, 1>^ combination_lastAdded = gcnew array<int, 1>(p); {
				for (int i = 0; i < p; i++) {
					combination_lastAdded[i] = i;
				}
			}
			ans->Add(combination_lastAdded);

			// Ajouter chaque combinaison à partir de la pécédente
			while (true) {
				array<int, 1>^ combination_new = gcnew array<int, 1>(p);
				// Déterminer la position à incrémenter (en partant de la fin)
				int pos; {
					pos = p - 1;
					while (pos >= 0 && combination_lastAdded[pos] == n - p + pos) {
						pos--;
					}
				}
				// La dernière combinaison ajoutée était-elle donc la dernière ? Si oui, arrêter
				if (pos == -1) break;
				// Recopier le début, incrémenter à la position demandée, et compléter la suite sur les positions suivantes
				for (int i = 0; i < pos; i++) combination_new[i] = combination_lastAdded[i];
				combination_new[pos] = combination_lastAdded[pos] + 1;
				for (int i = pos + 1; i < p; i++) combination_new[i] = combination_new[i - 1] + 1;
				// Ajouter à la liste de réponse et garder pour l'itération suivante
				ans->Add(combination_new);
				combination_lastAdded = combination_new;
			};

			// Retourner
			return ans;

		}
		static List<array<int, 1>^>^ getCombinations_sorting2(int p, int n) {

			// Récupérer les valeurs du premier algorithme
			array<array<int, 1>^, 1>^ ans = getCombinations_sorting1(p, n)->ToArray();

			// Trier pour faire apparaître progressivement les nouvelles notes
			Array::Sort(ans, gcnew Comparer_sorting2);

			// Retourner
			return gcnew List<array<int, 1>^>(ans);

		}
		static List<array<int, 1>^>^ getCombinations_sorting3(int p, int n) {

			// Récupérer les valeurs du premier algorithme
			array<array<int, 1>^, 1>^ ans = getCombinations_sorting1(p, n)->ToArray();

			// Trier au hasard
			scramble(ans);

			// Retourner
			return gcnew List<array<int, 1>^>(ans);

		}
	private:
		array<array<Pitch^, 1>^, 1>^ getNotesGroupsInMode(array<array<int, 1>^, 1>^ combinations, array<Pitch^, 1>^ pitches) {
			
			int nbGroups = combinations->Length;
			array<array<Pitch^, 1>^, 1>^ ans = gcnew array<array<Pitch^, 1>^, 1>(nbGroups);

			for (int i = 0; i < nbGroups; i++) {
				array<int, 1>^ combination = combinations[i];
				int nbNotesInCombination = combination->Length;
				array<Pitch^, 1>^ notesGroup = gcnew array<Pitch^, 1>(nbNotesInCombination);
				for (int j = 0; j < nbNotesInCombination; j++) {
					notesGroup[j] = pitches[combination[j]];
				}
				ans[i] = notesGroup;
			}

			return ans;
				
		}
		static array<array<Element^, 1>^, 1>^ getMusicalGroups(array<array<Pitch^, 1>^, 1>^ notesGroups, RhythmFigure^ baseRhythm, char figuresType, bool slurs) {
			
			int nbGroups = notesGroups->Length;
			array<array<Element^, 1>^, 1>^ ans = gcnew array<array<Element^, 1>^, 1>(nbGroups);
			
			for (int i = 0; i < nbGroups; i++) {
				if      (figuresType == 'c') ans[i] = getMusicalGroup_chord   (notesGroups[i], baseRhythm);
				else if (figuresType == 's') ans[i] = getMusicalGroup_sequence(notesGroups[i], baseRhythm, slurs);
				else                         throw gcnew Exception;
			}

			return ans;

		}
		static array<Element^, 1>^ getMusicalGroup_chord(array<Pitch^, 1>^ pitches, RhythmFigure^ baseRhythm) {

			return gcnew array<Element^, 1> {
				gcnew Note(
					baseRhythm,
					pitches
				)
			};

		}
		static array<Element^, 1>^ getMusicalGroup_sequence(array<Pitch^, 1>^ pitches, RhythmFigure^ baseRhythm, bool slurs) {
			
			int nbPitches = pitches->Length;
			array<Element^, 1>^ ans = gcnew array<Element^, 1>(nbPitches);

			for (int i = 0; i < nbPitches; i++) {
				NoteDecorations^ noteDecorations; {
					if      (nbPitches == 1 || slurs == false) noteDecorations = gcnew NoteDecorations;
					else if (i == 0                          ) noteDecorations = gcnew NoteDecorations(false, true, false);
					else if (i < nbPitches - 1               ) noteDecorations = gcnew NoteDecorations(false, false, false);
					else                                       noteDecorations = gcnew NoteDecorations(false, false, true);
				}
				ans[i] = gcnew Note(
					baseRhythm,
					pitches[i],
					noteDecorations
				);
			}

			return ans;

		}
	private:
		static Block^ createBlock(array<Element^, 1>^ elements, RhythmFigure^ baseRhythm, int nbBaseRhythmsPerBar) {
			
			int nbElements = elements->Length;
			Signature^ timeSignature = gcnew Signature(nbBaseRhythmsPerBar * baseRhythm->duration_1024th, nullptr);
			int nbBars = (nbElements + nbBaseRhythmsPerBar - 1) / nbBaseRhythmsPerBar;

			// Créer le block
			Block^ ans = gcnew Block(1);

			// Ajouter les mesures et le contenu
			for (int bar = 1; bar <= nbBars; bar++) {
				ans->addBar(timeSignature);
				array<Element^, 1>^ elementsInBar = gcnew array<Element^, 1>(nbBaseRhythmsPerBar);
				for (int i = 0; i < nbBaseRhythmsPerBar; i++) {
					int index = (bar - 1) * nbBaseRhythmsPerBar + i;
					elementsInBar[i] = (index < nbElements) ? elements[index] : gcnew Rest(baseRhythm);
				}
				ans[1, bar] = gcnew Group(elementsInBar);
			}

			// Retourner
			return ans;
			
		}

};
public ref class G_APSO_JumpGliss1 : public Generator {
	
	// Options
	private:
		// Notes
		static const int pitch1 = 36;
		static const int pitch2 = 74;
		static const int pitch3 = 39;
		static const int pitch4 = 89;
		static const int pitch5 = 54;
		static const int pitch6 = 98;
		static const int pitch7 = 44;
		static const int pitch8 = 83;
	private:
		// Temps
		static const int length = 98;
		static const int time1 = (int)Math::Round(length * 0.00);
		static const int time3 = (int)Math::Round(length * 0.38);
		static const int time5 = (int)Math::Round(length * 0.71);
		static const int time7 = (int)Math::Round(length * 0.93);
		static const int time8 = (int)Math::Round(length * 1.00);

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "APSO : Sauts en glissade 1";

		}
		virtual String^ description() override {
			
			return "Glissades successives en forme de paraboles.";

		}
		virtual ScoreAndInfos^ generate() override {

			// Générer les hauteurs
			array<int, 1>^ values1;
			array<int, 1>^ values2;
			int nb = time8 + 1;
			{
				// Récupérer les quatre glissades
				array<int, 1>^ a = getValues(1); a = reduceSize(a, a->Length - 1);
				array<int, 1>^ b = getValues(2); b = reduceSize(b, b->Length - 1);
				array<int, 1>^ c = getValues(3); c = reduceSize(c, c->Length - 1);
				array<int, 1>^ d = getValues(4);
				// Concaténer les deux glissades de la première voix
				array<int, 1>^ silence = fill(-1, time5);
				values1 = concat(silence, c, d);
				// Concaténer les trois glissades de la deuxième voix
				values2 = concat(a, b, d);
			}

			// Générer les notes des deux voix
			array<Element^, 1>^ notes1 = gcnew array<Element^, 1>(nb);
			array<Element^, 1>^ notes2 = gcnew array<Element^, 1>(nb);
			RhythmFigure^ rhythmFigure = RhythmFigure::_eighth;
			{
				for (int i = 0; i < nb; i++) {
					int v1 = values1[i];
					int v2 = values2[i];
					if (v1 == -1) {
						notes1[i] = gcnew Rest(rhythmFigure);
					} else {
						Pitch^ pitch1 = gcnew Pitch(v1);
						notes1[i] = gcnew Note(rhythmFigure, pitch1);
					}
					if (v2 == -1) {
						notes2[i] = gcnew Rest(rhythmFigure);
					} else {
						Pitch^ pitch2 = gcnew Pitch(v2);
						notes2[i] = gcnew Note(rhythmFigure, pitch2);
					}
				}
			}

			// Créer et remplir le block
			Block^ block = gcnew Block(2);
			block->addBar(gcnew Signature(nb * rhythmFigure->duration_1024th, nullptr));
			block[1, 1] = gcnew Group(notes1);
			block[2, 1] = gcnew Group(notes2);

			// Retourner
			return block;

		}

	// Fonctions
	private:
		static array<int, 1>^ getValues(int glissIndex) {
			
			int pitchBeg; int timeBeg;
			int pitchMid;
			int pitchEnd; int timeEnd;
			bool lin;

			if (glissIndex == 1) {
				pitchBeg = pitch1; timeBeg = time1;
				pitchMid = pitch2;
				pitchEnd = pitch3; timeEnd = time3;
				lin = false;
			} else if (glissIndex == 2) {
				pitchBeg = pitch3; timeBeg = time3;
				pitchMid = pitch4;
				pitchEnd = pitch7; timeEnd = time7;
				lin = false;
			} else if (glissIndex == 3) {
				pitchBeg = pitch5; timeBeg = time5;
				pitchMid = pitch6;
				pitchEnd = pitch7; timeEnd = time7;
				lin = false;
			} else if (glissIndex == 4) {
				pitchBeg = pitch7; timeBeg = time7;
				pitchMid = -1;
				pitchEnd = pitch8; timeEnd = time8;
				lin = true;
			} else {
				throw gcnew Exception;
			}

			return getValues(timeEnd - timeBeg, pitchBeg, pitchMid, pitchEnd, lin);

		}
		static array<int, 1>^ getValues(int length, int pitchBeg, int pitchMid, int pitchEnd, bool lin) {
		
			if (lin) return getValues_lin  (length, pitchBeg,           pitchEnd);
			else     return getValues_parab(length, pitchBeg, pitchMid, pitchEnd);

		}
		static array<int, 1>^ getValues_lin(int length, double pitchBeg, double pitchEnd) {
		
			array<int, 1>^ ans = gcnew array<int, 1>(length + 1);

			for (int i = 0; i <= length; i++) {
				double time01 = ((double)i) / length;
				ans[i] = (int)Math::Round(getValue_lin(time01, pitchBeg, pitchEnd));
			}

			return ans;

		}
		static array<int, 1>^ getValues_parab(int length, double pitchBeg, double pitchMid, double pitchEnd) {
		
			array<int, 1>^ ans = gcnew array<int, 1>(length + 1);

			for (int i = 0; i <= length; i++) {
				double time01 = ((double)i) / length;
				ans[i] = (int)Math::Round(getValue_parab(time01, pitchBeg, pitchEnd, pitchMid));
			}

			return ans;

		}
		static double getValue_lin(double t, double a, double b) {
			
			return a + t * (b - a);

		}
		static double getValue_parab(double t, double a, double b, double m) {
			
			double ma = m - a;
			double mb = m - b;
			double ba = b - a;
			double sqrt = 2 * Math::Sqrt(ma * mb);
			double B1 = 2 * ma + sqrt;
			double B2 = 2 * ma - sqrt;
			double A1 = ba - B1;
			double A2 = ba - B2;
			double posMax1 = -B1 / (2 * A1);
			double posMax2 = -B2 / (2 * A2);

			if (posMax1 >= 0 && posMax1 <= 1) {
				return (A1 * t + B1) * t + a;
			} else if (posMax2 >= 0 && posMax2 <= 1) {
				return (A2 * t + B2) * t + a;
			} else {
				throw gcnew Exception;
			}

		}

};
public ref class G_APSO_JumpGliss2 : public Generator {
	
	// Options
	private:
		// Notes
		static const int pitch1 = 36;
		static const int pitch2 = 69;
		static const int pitch3 = 52;
		static const int pitch4 = 75;
		static const int pitch5 = 58;
	private:
		// Temps
		static const int length_q = 21;             // nombre de noires
		static const int length_12 = 12 * length_q; // valeurs par noire
		static const int time1_12 = (int)Math::Round(length_12 * 0.00);
		static const int time3_12 = (int)Math::Round(length_12 * 0.42);
		static const int time5_12 = (int)Math::Round(length_12 * 1.00);
	private:
		// Ecrabouillage
		static const double ecrab1 = 1.1;
		static const double ecrab2 = 1.3;
	private:
		// Mode
		static array<int, 1>^ scale = gcnew array<int, 1> { 36, 39, 41, 43, 45, 47, 49, 52, 54, 56, 58, 60, 62, 65, 67, 69, 71, 73, 75, 78, 80, 82, 84, 86, 88, 91 };

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "APSO : Sauts en glissade 2";

		}
		virtual String^ description() override {
			
			return "Deux glissades successives en forme de paraboles écrasées au sommet.";

		}
		virtual ScoreAndInfos^ generate() override {

			// Générer les hauteurs (12 valeurs par noire, plus valeur d'arrivée)
			array<int, 1>^ valuesHi;
			array<int, 1>^ valuesLo;
			{
				// Récupérer les deux glissades hautes et les deux glissades basses
				array<int, 1>^ a = getValues(1, false, ecrab1, true ); a = reduceSize(a, a->Length - 1);
				array<int, 1>^ b = getValues(2, false, ecrab2, true );
				array<int, 1>^ c = getValues(1, true , ecrab1, false); c = reduceSize(c, c->Length - 1);
				array<int, 1>^ d = getValues(2, true , ecrab2, false);
				// Concaténer les deux glissades dans chaque voix
				valuesHi = concat(a, b);
				valuesLo = concat(c, d);
			}

			// Générer les notes (deux voix, réalisées en doubles croches et en triolets)
			array<Element^, 1>^ notesHi4perQ = gcnew array<Element^, 1>(length_12 / 3 + 1);
			array<Element^, 1>^ notesHi3perQ = gcnew array<Element^, 1>(length_12 / 12 + 1);
			array<Element^, 1>^ notesLo4perQ = gcnew array<Element^, 1>(length_12 / 3 + 1);
			array<Element^, 1>^ notesLo3perQ = gcnew array<Element^, 1>(length_12 / 12 + 1);
			{
				int nb = length_12 / 3;
				for (int i = 0; i <= nb; i++) {
					Pitch^ pitchHi = gcnew Pitch(valuesHi[3 * i]);
					Pitch^ pitchLo = gcnew Pitch(valuesLo[3 * i]);
					RhythmFigure^ rhythmFigure = i < nb ? RhythmFigure::_16th : RhythmFigure::_quarter;
					notesHi4perQ[i] = gcnew Note(rhythmFigure, pitchHi);
					notesLo4perQ[i] = gcnew Note(rhythmFigure, pitchLo);
				}
			}
			{
				int nb = length_12 / 12;
				for (int i = 0; i <= nb; i++) {
					TupletRatio^ ratio = gcnew TupletRatio(RhythmFigure::_eighth, 3, 2);
					if (i < nb) {
						Pitch^ pitchHi1 = gcnew Pitch(valuesHi[12 * i    ]);
						Pitch^ pitchLo1 = gcnew Pitch(valuesLo[12 * i    ]);
						Pitch^ pitchHi2 = gcnew Pitch(valuesHi[12 * i + 4]);
						Pitch^ pitchLo2 = gcnew Pitch(valuesLo[12 * i + 4]);
						Pitch^ pitchHi3 = gcnew Pitch(valuesHi[12 * i + 8]);
						Pitch^ pitchLo3 = gcnew Pitch(valuesLo[12 * i + 8]);
						notesHi3perQ[i] = gcnew Tuplet(ratio, gcnew array<NoteOrRest^, 1> {
							gcnew Note(RhythmFigure::_eighth, pitchHi1),
							gcnew Note(RhythmFigure::_eighth, pitchHi2),
							gcnew Note(RhythmFigure::_eighth, pitchHi3)
						});
						notesLo3perQ[i] = gcnew Tuplet(ratio, gcnew array<NoteOrRest^, 1> {
							gcnew Note(RhythmFigure::_eighth, pitchLo1),
							gcnew Note(RhythmFigure::_eighth, pitchLo2),
							gcnew Note(RhythmFigure::_eighth, pitchLo3)
						});
					} else {
						Pitch^ pitchHi = gcnew Pitch(valuesHi[12 * i]);
						Pitch^ pitchLo = gcnew Pitch(valuesLo[12 * i]);
						notesHi3perQ[i] = gcnew Note(RhythmFigure::_quarter, pitchHi);
						notesLo3perQ[i] = gcnew Note(RhythmFigure::_quarter, pitchLo);
					}
				}
			}

			// Créer et remplir le block
			Block^ block = gcnew Block(4);
			block->addBar(qSignature((length_12 / 12 + 1) + "/4"));
			block[1, 1] = gcnew Group(notesHi4perQ);
			block[2, 1] = gcnew Group(notesHi3perQ);
			block[3, 1] = gcnew Group(notesLo4perQ);
			block[4, 1] = gcnew Group(notesLo3perQ);

			// Retourner
			return block;

		}

	// Fonctions
	private:
		static array<int, 1>^ getValues(int glissIndex, bool lin, double ecrab, bool onMode) {
			
			int pitchBeg; int timeBeg;
			int pitchMid;
			int pitchEnd; int timeEnd;

			if (glissIndex == 1) {
				pitchBeg = pitch1; timeBeg = time1_12;
				pitchMid = pitch2;
				pitchEnd = pitch3; timeEnd = time3_12;
			} else if (glissIndex == 2) {
				pitchBeg = pitch3; timeBeg = time3_12;
				pitchMid = pitch4;
				pitchEnd = pitch5; timeEnd = time5_12;
			} else {
				throw gcnew Exception;
			}

			return getValues(timeEnd - timeBeg, pitchBeg, pitchMid, pitchEnd, lin, ecrab, onMode);

		}
		static array<int, 1>^ getValues(int length, int pitchBeg, int pitchMid, int pitchEnd, bool lin, double ecrab, bool onMode) {
			
			// Récupérer les valeurs
			array<double, 1>^ values_chromatic = lin
				? getValues_lin  (length, pitchBeg,           pitchEnd       )
				: getValues_parab(length, pitchBeg, pitchMid, pitchEnd, ecrab);
			int nb = values_chromatic->Length;

			// Plaquer sur le mode
			array<int, 1>^ ans = gcnew array<int, 1>(nb);
			for (int i = 0; i < nb; i++) {
				ans[i] = onMode
					? nearestInScale  (values_chromatic[i])
					: (int)Math::Round(values_chromatic[i]);
			}

			// Retourner
			return ans;

		}
		static int nearestInScale(double pitch) {
			
			double dist_nearest = Double::MaxValue;
			int nearest;
			double dist;

			for each (int pitchInScale in scale) {
				dist = Math::Abs(pitch - (double)pitchInScale);
				if (dist < dist_nearest) {
					dist_nearest = dist;
					nearest = pitchInScale;
				}
			}

			return nearest;

		}
	private:
		static array<double, 1>^ getValues_lin(int length, double pitchBeg, double pitchEnd) {
		
			array<double, 1>^ ans = gcnew array<double, 1>(length + 1);

			for (int i = 0; i <= length; i++) {
				double time01 = ((double)i) / length;
				ans[i] = getValue_lin(time01, pitchBeg, pitchEnd);
			}

			return ans;

		}
		static array<double, 1>^ getValues_parab(int length, double pitchBeg, double pitchMid, double pitchEnd, double ecrab) {
		
			array<double, 1>^ ans = gcnew array<double, 1>(length + 1);

			for (int i = 0; i <= length; i++) {
				double time01 = ((double)i) / length;
				ans[i] = getValue_parab(time01, pitchBeg, pitchEnd, pitchMid, ecrab);
			}

			return ans;

		}
	private:
		static double getValue_lin(double t, double a, double b) {
			
			return a + t * (b - a);

		}
		static double getValue_parab(double t, double a, double b, double m, double ecrab) {
			
			double ma = m - a;
			double mb = m - b;
			double ba = b - a;
			double sqrt = 2 * Math::Sqrt(ma * mb);
			double B1 = 2 * ma + sqrt;
			double B2 = 2 * ma - sqrt;
			double A1 = ba - B1;
			double A2 = ba - B2;
			double posMax1 = -B1 / (2 * A1);
			double posMax2 = -B2 / (2 * A2);

			double A, B, posMax; {
				if      (posMax1 >= 0 && posMax1 <= 1) { A = A1; B = B1; posMax = posMax1; }
				else if (posMax2 >= 0 && posMax2 <= 1) { A = A2; B = B2; posMax = posMax2; }
				else                                   { throw gcnew Exception;            }
			}

			// Récupérer la valeur de la parabole
			double parabValue = (A * t + B) * t + a;

			// Ecraser 'parabValue'
			{
				double min, max;
				if (t <= posMax) { min = Math::Min(a, m); max = Math::Max(a, m); }
				else             { min = Math::Min(m, b); max = Math::Max(m, b); }
				// Mettre 'parabValue' entre 0 et 1
				parabValue = (parabValue - min) / (max - min);
				if (parabValue < 0 || parabValue > 1) throw gcnew Exception;
				// Ecraser 'parabValue'
				parabValue = 1 - Math::Pow(1 - parabValue, ecrab);
				// Remettre 'parabValue' entre 'min' et 'max'
				parabValue = min + (max - min) * parabValue;
			}

			// Retourner
			return parabValue;

		}

};
public ref class G_APSO_GrandMarkov : public G_MarkovOnBlocks {

	// Constructeur (pour randomiser toujours à la même valeur)
	public:
		G_APSO_GrandMarkov() {
			
			Rand::randomize(0);

		}

	// Implémentations
	public:
		// Générateur
		virtual String^ name() override {
			
			return "APSO : Grande chaîne de Markov évolutive";

		}
		virtual String^ description() override {
			
			return "Chaîne de Markov décrivant l'évolution des motifs entre les petits coups de deux types et Sierpinski.";

		}
	protected:
		// Chaîne de Markov
		virtual int getOrder() override {
			
			return 5;

		}
		virtual List<TimeAndMatrix^>^ getProbaStages() override {
			
			List<TimeAndMatrix^>^ ans = gcnew List<TimeAndMatrix^>;

			ans->Add(gcnew TimeAndMatrix(
				0, gcnew array<double, 2> {
				{ 0, 0.66, 0.33, 0, 0 },
				{ 0.5, 0.16, 0.33, 0, 0 },
				{ 0.75, 0, 0.25, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 }
			}));
			ans->Add(gcnew TimeAndMatrix(
				0.1,
				ans[ans->Count - 1]->matrix
			));
			ans->Add(gcnew TimeAndMatrix(
				0.8, gcnew array<double, 2> {
				{ 0.2, 0.2, 0.2, 0.2, 0.2 },
				{ 0.33, 0, 0.33, 0.33, 0 },
				{ 0.25, 0, 0.25, 0.25, 0.25 },
				{ 0.29, 0.14, 0.29, 0, 0.29 },
				{ 0.16, 0.17, 0.33, 0.33, 0 }
			}));
			ans->Add(gcnew TimeAndMatrix(
				1,
				ans[ans->Count - 1]->matrix
			));

			return ans;

		}
		virtual array<BlocksGeneratorForMarkov^, 1>^ getGenerators() override {
			
			return gcnew array<BlocksGeneratorForMarkov^, 1> {
				gcnew BlocksGenerator0,
				gcnew BlocksGenerator1,
				gcnew BlocksGenerator2,
				gcnew BlocksGenerator3,
				gcnew BlocksGenerator4
			};

		}
		virtual array<int, 1>^ getPossibleInitialValues() override {
			
			return newArray(0);

		}
		virtual int getMinDuration_1024th() override {
			
			int bars_C = 35;

			return 1024 * bars_C;

		}
		virtual int getFinalValue_or_m1() override {
			
			return 4;

		}
		virtual bool getSquareProbas() override {

			return true;

		}
		virtual ScoreAndInfos^ doOnGeneratedBlock(Block^ block) override {
			
			// Remplir tous les trous des parties de wood blocks 2 et 3 avec Thue
			fillWoodBlocks23(block);

			// Ajouter des commentaires et retourner
			String^ comments; {
				comments =
					"mf joueur" + "\r\n" +
					"cresc à partir d'un certain point" + "\r\n" +
					"accelerando (idem)" + "\r\n" +
					"ajouter le rythme de Thue (restant de la partie précédente)" + "\r\n" +
					"ajouter mélodie violons";
			}
			return gcnew ScoreAndInfos(block, comments);

		}

	// Classes de chaque générateur
	private:
		static const int nbParts = 63;
		static const int part_fl = 1, part_hb = 4, part_cl = 7, part_bsn = 10;
		static const int part_cor = 13, part_tpt = 17, part_tbn = 20, part_tub = 23;
		static const int part_xyl = 25, part_wbl = 27, part_bum = 30;
		static const int part_hp = 31, part_pno = 33;
		static const int part_v1 = 35, part_v2 = 43, part_al = 50, part_vcl = 56, part_cb = 61;
	private:
		ref class BlocksGenerator0 : public BlocksGeneratorForMarkov {

			// Implémentation
			public:
				virtual Block^ generate(double time01, int indexInGlobalSeq, int indexOfThisType) override {

					// PROV
					//Block^ bblock = gcnew Block(nbParts);
					//bblock->addBar(gcnew Signature("1/4"));
					//return bblock;





					// Choisir le type de rythme (1, 2 ou 3), et en déduire le nombre de croches dans la mesure
					int ryType;
					int nbEighths;
					{
						// Type de rythme
						if      (time01 < 0.2) ryType = 1;
						else if (time01 < 0.4) ryType = Rand::chooseIn(newArray(1, 2));
						else if (time01 < 0.6) ryType = Rand::chooseIn(newArray(1, 2, 2, 2));
						else if (time01 < 0.8) ryType = Rand::chooseIn(newArray(2, 3));
						else                    ryType = Rand::chooseIn(newArray(2, 3));
						// Nombre de croches dans la mesure
						if      (ryType == 1) nbEighths = 2;
						else if (ryType == 2) nbEighths = 1;
						else if (ryType == 3) nbEighths = 3;
					}

					// Choisir les notes utilisées pour l'accord aigu (qui est souvent le seul)
					array<int, 1>^ pitchesHi; {
						// Choisir le mode (1 ou 2)
						int mode; {
							if      (time01 < 0.33) mode = 1;
							else if (time01 < 0.67) mode = Rand::chooseIn(newArray(1, 2));
							else                     mode = Rand::chooseIn(newArray(1, 2, 2));
						}
						// Choisir le renversement (1, 2 ou 3)
						int renv; {
							if      (time01 < 0.25) renv = 1;
							else if (time01 < 0.6 ) renv = Rand::chooseIn(newArray(1, 2));
							else                     renv = Rand::chooseIn(newArray(1, 2, 3));
							if (renv == 3 && mode == 1) renv = Rand::chooseIn(newArray(1, 2)); // Pas de 3ème renversement pour le 1er mode
						}
						// Récupérer les notes du mode dans 'pitchesHi'
						if      (mode == 1 && renv == 1) pitchesHi = newArray(69, 74, 78, 80, 84, 87, 89);
						else if (mode == 1 && renv == 2) pitchesHi = newArray(72, 74, 75, 77, 78, 79, 81);
						else if (mode == 2 && renv == 1) pitchesHi = newArray(73, 74, 76, 77, 78, 79, 81, 84);
						else if (mode == 2 && renv == 2) pitchesHi = newArray(67, 73, 74, 76, 77, 78, 81, 84);
						else if (mode == 2 && renv == 3) pitchesHi = newArray(66, 69, 72, 79, 85, 86, 88, 89);
						// Choisir la transposition en demi-tons --> entre -6 et +4, mais toujours 0 au début
						int transp; {
							double when = 0.25;
							if (time01 < when) {
								transp = 0;
							} else {
								double timetime = (time01 - when) / (1 - when);
								double transptransp = (Rand::double01() * 2 - 1) * timetime;
								if (transptransp < 0) transp = (int)Math::Round(transptransp * 6.49);
								else                  transp = (int)Math::Round(transptransp * 4.49);
							}
						}
						// Transposer 'pitchesHi'
						{
							int nb = pitchesHi->Length;
							for (int i = 0; i < nb; i++) {
								pitchesHi[i] += transp;
							}
						}
					}

					// Choisir les notes utilisées pour l'accord grave (seulement si le rythme est le n°3)
					array<int, 1>^ pitchesLo; {
						int basePitch;
						if (time01 < 0.85) basePitch = Rand::integer(56, 62);
						else                basePitch = Rand::integer(44, 56);
						int nb = Rand::integer(7, 13);
						pitchesLo = gcnew array<int, 1>(nb);
						for (int i = 0; i < nb; i++) {
							pitchesLo[i] = basePitch + i;
						}
					}

					// Créer les éléments pour chaque partie
					array<List<Element^>^, 1>^ out; {
						// Créer une liste vide d'éléments pour chaque partie
						out = gcnew array<List<Element^>^, 1>(nbParts + 1);
						for (int part = 1; part <= nbParts; part++) out[part] = gcnew List<Element^>;
						// Ajouter les éléments
						if (ryType == 1) {
							bool xyloOverruns = addHi_returnXyloOverruns(out, pitchesHi, ryType, time01);
							addRest(out, xyloOverruns);
						} else if (ryType == 2) {
							addHi_returnXyloOverruns(out, pitchesHi, ryType, time01);
						} else if (ryType == 3) {
							addLo(out, pitchesLo, time01);
							bool xyloOverruns = addHi_returnXyloOverruns(out, pitchesHi, ryType, time01);
							addRest(out, xyloOverruns);
						}
					}

					// Si les éléments d'une partie sont tous des silences, convertir en pause
					for (int part = 1; part <= nbParts; part++) {
						bool allRests = true;
						int durSum_1024th = 0;
						for each (Element^ element in out[part]) {
							if (!element->isRest()) {
								allRests = false;
								break;
							}
							durSum_1024th += element->duration_1024th;
						}
						if (allRests) {
							Element^ wholeBarRest = Rest::wholeBarRest(durSum_1024th);
							out[part] = gcnew List<Element^>(gcnew array<Element^> { wholeBarRest });
						}
					}

					// Créer le block, ajouter et retourner
					Block^ block = gcnew Block(nbParts);
					block->addBar(qSignature(nbEighths + "/8"));
					for (int part = 1; part <= nbParts; part++) {
						block[part, 1] = gcnew Group(out[part]);
					}
					return block;

				}
			
			// Fonctions
			private:
				bool addHi_returnXyloOverruns(array<List<Element^>^, 1>^ out, array<int, 1>^ pitches, int ryType, double time01) {
				
					// Déterminer les parties concernées par la répartition, du grave vers l'aigu
					List<int>^ partsRepart = gcnew List<int>(gcnew array<int, 1> {
						part_fl, part_fl + 1, part_fl + 2,
						part_hb, part_hb + 1, part_hb + 2,
						part_tpt, part_tpt + 1, part_tpt + 2
					});
					partsRepart->Reverse();

					// Décider si on ajoute des appoggiatures, et si oui à quelle proportion des notes de l'accord
					bool graceNotes;
					double propGraceNotes;
					{
						if (ryType == 2) {
							graceNotes = Rand::double01() < 0.2;
						} else {
							if      (time01 < 0.25) graceNotes = Rand::double01() < 0.25;
							else if (time01 < 0.5 ) graceNotes = Rand::double01() < 0.7 ;
							else if (time01 < 0.75) graceNotes = Rand::double01() < 0.2 ;
							else                     graceNotes = Rand::double01() < 0.85;
						}
						if (graceNotes) {
							propGraceNotes = Math::Sqrt(Rand::double01()) * 0.5 + 0.5;
						}
					}

					// Ajouter les notes 'pitches' à toutes ces parties, de façon à couvrir le mieux possible l'accord demandé
					addNotes(out, pitches, partsRepart->ToArray(), graceNotes, propGraceNotes);

					// Ajouter toutes les notes au piano, toutes les notes bécarres à la harpe, et un groupe de notes au xylo
					addPianoNotes(out, pitches);
					addHarpNotes (out, pitches);
					bool xyloOverrun = addXyloNotes_returnOverrun(out, pitches, ryType);

					// Déterminer quelles parties de cordes sont concernées par une répartition au hasard
					List<int>^ partsStrings = gcnew List<int>; {
						double prop;
						if      (time01 < 0.45) prop = 0;
						else if (time01 < 0.85) prop = (time01 - 0.45) / (0.85 - 0.45);
						else                     prop = 1;
						int minStringPart = part_v1; // Que les violons et les altos
						int maxStringPart = part_vcl - 1;
						for (int part = minStringPart; part <= maxStringPart; part++) {
							if (Rand::double01() < prop) partsStrings->Add(part);
						}
					}

					// Ajouter toutes les notes aux cordes
					addStringNotes(out, partsStrings, pitches);

					// Déterminer toutes les parties non encore remplies, et leur ajouter un demi-soupir
					for (int part = 1; part <= nbParts; part++) {
						if (!contains(partsRepart, part) && !contains(partsStrings, part)) {
							if (part != part_pno && part != part_pno + 1 && part != part_hp && part != part_hp + 1 && part != part_xyl) {
								out[part]->Add(individualRest());
							}
						}
					}
					
					// Retourner
					return xyloOverrun;

				}
				void addLo(array<List<Element^>^, 1>^ out, array<int, 1>^ pitches, double time01) {
				
					// Déterminer les parties concernées par la répartition, du grave vers l'aigu
					List<int>^ partsRepart = gcnew List<int>(gcnew array<int, 1> {
						part_cl, part_cl + 1, part_cl + 2,
						part_bsn, part_bsn + 1, part_bsn + 2,
						part_cor, part_cor + 1, part_cor + 2, part_cor + 3,
						part_tbn, part_tbn + 1, part_tbn + 2
					});
					partsRepart->Reverse();

					// Ajouter les notes 'pitches' à toutes ces parties, de façon à couvrir le mieux possible l'accord demandé
					addNotes(out, pitches, partsRepart->ToArray(), false, 0);

					// Ajouter la basse au piano, la note aigue à la harpe, et un coup de grosse caisse
					out[part_pno + 1]->Add(individualNote(pitches[0], false));
					out[part_hp     ]->Add(individualNote(pitches[pitches->Length - 1], false));
					Pitch^ pitch_bum = gcnew Pitch(71);
					out[part_bum    ]->Add(gcnew Note(RhythmFigure::_eighth, pitch_bum));

					// Déterminer quelles parties de cordes sont concernées par une répartition au hasard
					List<int>^ partsStrings = gcnew List<int>; {
						double prop;
						if      (time01 < 0.6 ) prop = 0;
						else if (time01 < 0.85) prop = (time01 - 0.6) / (0.85 - 0.6);
						else                     prop = 1;
						int minStringPart = part_al; // Que les contrebasses, les violoncelles et les altos
						int maxStringPart = part_cb + 2;
						for (int part = minStringPart; part <= maxStringPart; part++) {
							if (Rand::double01() < prop) partsStrings->Add(part);
						}
					}

					// Ajouter toutes les notes aux cordes
					addStringNotes(out, partsStrings, pitches);

					// Déterminer toutes les parties non encore remplies, et leur ajouter un demi-soupir
					for (int part = 1; part <= nbParts; part++) {
						if (!contains(partsRepart, part) && !contains(partsStrings, part)) {
							if (part != part_pno + 1 && part != part_hp && part != part_bum) {
								out[part]->Add(individualRest());
							}
						}
					}
				
				}
				void addRest(array<List<Element^>^, 1>^ out, bool xyloOverruns) {
				
					for (int part = 1; part <= nbParts; part++) {
						if (!(part == part_xyl && xyloOverruns)) {
							out[part]->Add(individualRest());
						}
					}
				
				}
			private:
				void addNotes(array<List<Element^>^, 1>^ out, array<int, 1>^ pitches, array<int, 1>^ parts, bool graceNotes, double propGraceNotes) {

					int _nbPitches = pitches->Length;
					int _nbParts = parts->Length;

					// Déterminer le nombre de parties où se retrouvera chaque pitch
					array<int, 1>^ nbOccurrencesOfPitch; {
						int forAll = _nbParts / _nbPitches;
						nbOccurrencesOfPitch = fill(forAll, _nbPitches);
						int remaining = _nbParts - forAll * _nbPitches;
						for (int i = 0; i < remaining; i++) {
							// Incrémenter à une position aléatoire non encore augmentée
							int index;
							do {
								index = Rand::integer(0, _nbPitches);
							} while (nbOccurrencesOfPitch[index] > forAll);
							nbOccurrencesOfPitch[index]++;
						}
					}

					// Ajouter
					int pitchIndex = 0;
					for each (int part in parts) {
						if (nbOccurrencesOfPitch[pitchIndex] == 0) pitchIndex++;
						// Déterminer les éléments à ajouter
						List<Element^>^ elementsToAdd = gcnew List<Element^>; {
							// Déterminer le pitch
							int pitch = pitches[pitchIndex];
							// Décider si cet élément a des appoggiatures
							bool graceNotesHere = graceNotes && Rand::double01() < propGraceNotes;
							// Ajouter les appoggiatures éventuelles
							if (graceNotesHere) {
								int nbGraceNotes;
								if (part >= part_tpt && part <= part_tub) {
									nbGraceNotes = 1;
								} else {
									nbGraceNotes = Rand::chooseIn(newArray(1, 1, 2, 2, 2, 2, 3));
								}
								array<int, 1>^ graceNotesPitches; {
									if (nbGraceNotes == 1) {
										graceNotesPitches = gcnew array<int, 1> { pitch - 2 };
									} else if (nbGraceNotes == 2) {
										graceNotesPitches = Rand::chooseIn(newArray(
											gcnew array<int, 1> { pitch - 2, pitch - 1 },
											gcnew array<int, 1> { pitch - 3, pitch - 2 },
											gcnew array<int, 1> { pitch - 3, pitch - 1 }
										));								} else if (nbGraceNotes == 3) {
										graceNotesPitches = gcnew array<int, 1> { pitch - 3, pitch - 2, pitch - 1 };
									}
								}
								for (int i = 0; i < nbGraceNotes; i++) {
									Pitch^ pitchpitch = gcnew Pitch(graceNotesPitches[i]);
									elementsToAdd->Add(Note::graceNote(
										RhythmFigure::_16th,
										pitchpitch,
										gcnew NoteDecorations(false, i == 0, false)
									));
								}
							}
							// Ajouter la note principale
							elementsToAdd->Add(individualNote(pitch, graceNotesHere));
						}
						// Ajouter les éléments
						for each (Element^ elementToAdd in elementsToAdd) {
							out[part]->Add(elementToAdd);
						}
						// Décrémenter
						nbOccurrencesOfPitch[pitchIndex]--;
					}

				}
				void addPianoNotes(array<List<Element^>^, 1>^ out, array<int, 1>^ pitches) {
					
					// Couper les notes en deux parties
					array<int, 1>^ pitches_lo;
					array<int, 1>^ pitches_hi;
					{
						List<int>^ pitches_lo_ = gcnew List<int>;
						List<int>^ pitches_hi_ = gcnew List<int>;
						int nbPitches = pitches->Length;
						for (int i = 0; i < nbPitches; i++) {
							if (i < nbPitches / 2) pitches_lo_->Add(pitches[i]);
							else                   pitches_hi_->Add(pitches[i]);
						}
						pitches_lo = pitches_lo_->ToArray();
						pitches_hi = pitches_hi_->ToArray();
					}

					// Eviter les problèmes de mains de pianistes
					{
						if (pitches_lo->Length > 0) {
							while (pitches_lo[pitches_lo->Length - 1] - pitches_lo[0] > 13 || pitches_lo->Length > 5) {
								bool rev = Rand::integer(0, 2) == 0;
								if (rev) Array::Reverse(pitches_lo);
								pitches_lo = reduceSize(pitches_lo, pitches_lo->Length - 1);
								if (rev) Array::Reverse(pitches_lo);
							}
						}
						if (pitches_hi->Length > 0) {
							while (pitches_hi[pitches_hi->Length - 1] - pitches_hi[0] > 13 || pitches_hi->Length > 5) {
								bool rev = Rand::integer(0, 2) == 0;
								if (rev) Array::Reverse(pitches_hi);
								pitches_hi = reduceSize(pitches_hi, pitches_hi->Length - 1);
								if (rev) Array::Reverse(pitches_hi);
							}
						}
					}

					// Ajouter
					out[part_pno    ]->Add(pitches_hi->Length >= 1 ? individualNote(pitches_hi, false) : individualRest());
					out[part_pno + 1]->Add(pitches_lo->Length >= 1 ? individualNote(pitches_lo, false) : individualRest());

				}
				void addHarpNotes(array<List<Element^>^, 1>^ out, array<int, 1>^ pitches) {
					
					// Garder seulement les bécarres
					array<int, 1>^ pitches_bec; {
						List<int>^ pitches_bec_lst = gcnew List<int>;
						for each (int pitch in pitches) {
							int mod = pitch % 12;
							if (mod == 0 || mod == 2 || mod == 4 || mod == 5 || mod == 7 || mod == 9 || mod == 11) pitches_bec_lst->Add(pitch);
						}
						pitches_bec = pitches_bec_lst->ToArray();
					}

					// Couper les notes en deux parties, en gardant seulement les bécarres
					array<int, 1>^ pitches_lo;
					array<int, 1>^ pitches_hi;
					{
						List<int>^ pitches_lo_ = gcnew List<int>;
						List<int>^ pitches_hi_ = gcnew List<int>;
						int nbPitches = pitches_bec->Length;
						for (int i = 0; i < nbPitches; i++) {
							if (i < nbPitches / 2) pitches_lo_->Add(pitches_bec[i]);
							else                   pitches_hi_->Add(pitches_bec[i]);
						}
						pitches_lo = pitches_lo_->ToArray();
						pitches_hi = pitches_hi_->ToArray();
					}

					// Eviter les problèmes de mains de harpistes
					{
						if (pitches_lo->Length > 0) {
							while (pitches_lo[pitches_lo->Length - 1] - pitches_lo[0] > 13 || pitches_lo->Length > 4) {
								bool rev = Rand::integer(0, 2) == 0;
								if (rev) Array::Reverse(pitches_lo);
								pitches_lo = reduceSize(pitches_lo, pitches_lo->Length - 1);
								if (rev) Array::Reverse(pitches_lo);
							}
						}
						if (pitches_hi->Length > 0) {
							while (pitches_hi[pitches_hi->Length - 1] - pitches_hi[0] > 13 || pitches_hi->Length > 4) {
								bool rev = Rand::integer(0, 2) == 0;
								if (rev) Array::Reverse(pitches_hi);
								pitches_hi = reduceSize(pitches_hi, pitches_hi->Length - 1);
								if (rev) Array::Reverse(pitches_hi);;
							}
						}
					}

					// Ajouter
					out[part_hp    ]->Add(pitches_hi->Length >= 1 ? individualNote(pitches_hi, false) : individualRest());
					out[part_hp + 1]->Add(pitches_lo->Length >= 1 ? individualNote(pitches_lo, false) : individualRest());

				}
				bool addXyloNotes_returnOverrun(array<List<Element^>^, 1>^ out, array<int, 1>^ pitches, int ryType) {
					
					// Choisir une note au hasard dans les notes proposées, jouable au xylophone
					Pitch^ pitchChoice; {
						int choice;
						do {
							choice = Rand::chooseIn(pitches);
						} while (choice < 65);
						pitchChoice = gcnew Pitch(choice);
					}

					// Ajouter
					if (ryType == 2) {
						out[part_xyl]->Add(gcnew Note(RhythmFigure::_16th, pitchChoice));
						out[part_xyl]->Add(gcnew Note(RhythmFigure::_16th, pitchChoice));
						return false;
					} else {
						bool isTuplet = (ryType == 1 ? Rand::double01() < 0.15 : Rand::double01() < 0.5);
						if (isTuplet) {
							out[part_xyl]->Add(gcnew Tuplet(gcnew TupletRatio(RhythmFigure::_16th, 3, 2), gcnew array<NoteOrRest^, 1> {
								gcnew Note(RhythmFigure::_16th, pitchChoice),
								gcnew Note(RhythmFigure::_16th, pitchChoice),
								gcnew Note(RhythmFigure::_16th, pitchChoice)
							}));
							out[part_xyl]->Add(gcnew Note(RhythmFigure::_eighth, pitchChoice));
						} else {
							out[part_xyl]->Add(gcnew Note(RhythmFigure::_16th, pitchChoice));
							out[part_xyl]->Add(gcnew Note(RhythmFigure::_16th, pitchChoice));
							out[part_xyl]->Add(gcnew Note(RhythmFigure::_16th, pitchChoice));
							out[part_xyl]->Add(gcnew Note(RhythmFigure::_16th, pitchChoice));
						}
						return true;
					}

				}
				void addStringNotes(array<List<Element^>^, 1>^ out, List<int>^ parts, array<int, 1>^ pitches) {
					
					for each (int part in parts) {
						// Déterminer l'instrument (1 = violon, 2 = alto, 3 = violoncelle, 4 = contrebasse)
						int instr; {
							if      (part < part_al ) instr = 1;
							else if (part < part_vcl) instr = 2;
							else if (part < part_cb ) instr = 3;
							else                      instr = 4;
						}
						// Déterminer les limites jouables
						int minPossible, maxPossible; {
							if (instr == 1) {
								minPossible = 55;
								maxPossible = 91;
							} else if (instr == 2) {
								minPossible = 48;
								maxPossible = 81;
							} else if (instr == 3) {
								minPossible = 36;
								maxPossible = 69;
							} else if (instr == 4) {
								minPossible = 28;
								maxPossible = 47;
							}
						}
						// Trouver les notes jouables
						List<int>^ playablePitches = gcnew List<int>;
						int nbPlayablePitches;
						{
							for each (int pitch in pitches) {
								if (pitch >= minPossible && pitch <= maxPossible) {
									playablePitches->Add(pitch);
								}
							}
							nbPlayablePitches = playablePitches->Count;
						}
						// Choisir la note
						int pitch; {
							if (nbPlayablePitches == 0) {
								pitch = Rand::chooseIn(pitches);
								while (pitch < minPossible) pitch += 12;
								while (pitch > maxPossible) pitch -= 12;
							} else {
								pitch = Rand::chooseIn(playablePitches);
							}
						}
						// Ajouter
						out[part]->Add(individualNote(pitch, false));
					}

				}
				Element^ individualRest() {
					
					return gcnew Rest(RhythmFigure::_eighth);

				}
				Element^ individualNote(array<int, 1>^ pitches, bool slurEnd) {
					
					int nbPitches = pitches->Length;
					array<Pitch^, 1>^ pitches_ = gcnew array<Pitch^, 1>(nbPitches);
					for (int i = 0; i < nbPitches; i++) pitches_[i] = gcnew Pitch(pitches[i]);

					NoteDecorations^ decorations = gcnew NoteDecorations(Articulation::staccato, false, false, slurEnd);

					return gcnew Note(RhythmFigure::_eighth, pitches_, decorations);

				}
				Element^ individualNote(int pitch, bool slurEnd) {
					
					return individualNote(newArray(pitch), slurEnd);

				}

		};
		ref class BlocksGenerator1 : public BlocksGeneratorForMarkov {

			// Implémentation
			public:
				virtual Block^ generate(double time01, int indexInGlobalSeq, int indexOfThisType) override {

					// PROV
					//Block^ bblock = gcnew Block(nbParts);
					//bblock->addBar(gcnew Signature("1/4"));
					//return bblock;
					





					// Choisir le nombre de croches de la tenue
					int nbEighths;
					{
						double min, max;
						if (time01 <= 0.5) {
							double time = 2 * time01;
							min = 4 - 2 * time;
							max = 8 - 4 * time;
						} else {
							double time = 2 * time01 - 1;
							min = 2 +     time;
							max = 4 + 6 * time;
						}
						min -= 0.49;
						max += 0.49;
						nbEighths = (int)Math::Round(min + (max - min) * Rand::double01());
					}

					// Choisir les notes utilisées pour l'accord
					array<int, 1>^ pitches; {
						// Choisir le nombre d'étages (2 ou 3)
						int stages; {
							if      (time01 < 0.3) stages = 2;
							else if (time01 < 0.8) stages = (Rand::double01() < (time01 - 0.3) / (0.8 - 0.3)) ? 3 : 2;
							else                   stages = 3;
						}
						// Choisir les inversions
						bool inv1 = true;
						bool inv2 = (indexOfThisType == 0) ? false : Rand::chooseIn(newArray(false, true));
						bool inv3 = false;
						// Récupérer les notes du mode dans 'pitches'
						{
							List<int>^ pitches_lst = gcnew List<int>;
							// Etage 1
							pitches_lst->Add(43 + (inv1 ? 1 : 0));
							pitches_lst->Add(48);
							pitches_lst->Add(52);
							pitches_lst->Add(56 - (inv1 ? 1 : 0));
							// Etage 2
							pitches_lst->Add(58 - (inv2 ? 1 : 0));
							pitches_lst->Add(61);
							pitches_lst->Add(66);
							pitches_lst->Add(69 + (inv2 ? 1 : 0));
							// Etage 3
							if (stages == 3) {
								pitches_lst->Add(74 + (inv3 ? 1 : 0));
								pitches_lst->Add(77);
								pitches_lst->Add(83);
								pitches_lst->Add(87 - (inv3 ? 1 : 0));
							}
							// Enregistrer
							pitches = pitches_lst->ToArray();
						}
						// Choisir la transposition en demi-tons --> toujours en tierces mineures, entre -6 et +6, mais toujours 0 au début
						int transp; {
							if      (time01 < 0.3) transp = 0;
							else if (time01 < 0.5) transp = Rand::chooseIn(newArray( 0, 3));
							else if (time01 < 0.7) transp = Rand::chooseIn(newArray(-3, 3));
							else                   transp = Rand::chooseIn(newArray(-6, 6));
						}
						// Transposer 'pitches'
						{
							int nb = pitches->Length;
							for (int i = 0; i < nb; i++) {
								pitches[i] += transp;
							}
						}
					}

					// Créer les éléments pour chaque partie
					array<List<Element^>^, 1>^ out; {
						// Créer une liste vide d'éléments pour chaque partie
						out = gcnew array<List<Element^>^, 1>(nbParts + 1);
						for (int part = 1; part <= nbParts; part++) out[part] = gcnew List<Element^>;
						// Ajouter
						fillParts(out, pitches, nbEighths, time01);
					}

					// Créer le block, ajouter et retourner
					Block^ block = gcnew Block(nbParts);
					block->addBar(gcnew Signature(nbEighths * 128, nullptr));
					for (int part = 1; part <= nbParts; part++) {
						block[part, 1] = gcnew Group(out[part]);
					}
					return block;

				}

			// Fonctions
			private:
				void fillParts(array<List<Element^>^, 1>^ out, array<int, 1>^ pitches, int nbEighths, double time01) {
					
					int nbPitches = pitches->Length;

					// Déterminer les parties qui réalisent l'accord, de la plus grave à la plus aiguë
					array<int, 1>^ parts; {
						if (nbPitches == 8) {
							parts = newArray(
								part_cor, part_cor + 1, part_cor + 2, part_cor + 3,
								part_tbn, part_tbn + 1, part_tbn + 2,
								part_tub
							);
						} else if (nbPitches == 12) {
							parts = newArray(
								part_fl, part_fl + 1,
								part_hb + 1, part_hb + 2,
								part_cor, part_cor + 1, part_cor + 2, part_cor + 3,
								part_tbn, part_tbn + 1, part_tbn + 2,
								part_tub
							);
						} else {
							throw gcnew Exception;
						}
						Array::Reverse(parts);
					}

					// Ajouter l'accord à ces parties
					for (int partIndex = 0; partIndex < nbPitches; partIndex++) {
						int part = parts[partIndex];
						Pitch^ pitch = gcnew Pitch(pitches[partIndex]);
						array<Note^, 1>^ tiedNotes = Note::getSum(pitch, nbEighths * 128);
						{
							// Changer la première pour mettre une nuance
							auto rhythm  = tiedNotes[0]->rhythm;
							auto pitches = tiedNotes[0]->pitches;
							auto deco    = tiedNotes[0]->decorations;
							auto words   = tiedNotes[0]->words;
							array<Words::Words^, 1>^ newWords = concat(words, newArray((Words::Words^)(gcnew Words::Dynamic("sfz pp"))));
							tiedNotes[0] = gcnew Note(rhythm, pitches, deco, newWords);
						}
						for each (Element^ element in tiedNotes) out[part]->Add(element);
					}

					// Ajouter éventuellement des appogiatures du hautbois 1 et des clarinettes
					if (nbPitches == 12) {
						addGraceNotes(out, pitches, part_cl    , 11, nbEighths);
						addGraceNotes(out, pitches, part_cl + 1, 10, nbEighths);
						addGraceNotes(out, pitches, part_cl + 2,  9, nbEighths);
						addGraceNotes(out, pitches, part_hb    ,  8, nbEighths);
					}

					// Ajouter des pizz. de contrebasses
					{
						Rhythm^ r; {
							if      (nbEighths >= 7) r = RhythmFigure::_half;
							else if (nbEighths == 6) r = gcnew Rhythm(RhythmFigure::_half, 1);
							else if (nbEighths >= 4) r = RhythmFigure::_half;
							else if (nbEighths == 3) r = gcnew Rhythm(RhythmFigure::_quarter, 1);
							else if (nbEighths == 2) r = RhythmFigure::_quarter;
							else if (nbEighths == 1) r = RhythmFigure::_eighth;
						}
						array<Rest^, 1>^ remaining = Rest::getSum(nbEighths * 128 - r->duration_1024th);
						out[part_cb + 2]->Add(gcnew Note(r, newArray(gcnew Pitch(pitches[0] - 12)), newArray((Words::Words^)(gcnew Words::Normal("pizz.")))));
						out[part_cb + 1]->Add(gcnew Note(r, newArray(gcnew Pitch(pitches[2] - 12)), newArray((Words::Words^)(gcnew Words::Normal("pizz.")))));
						out[part_cb    ]->Add(gcnew Note(r, newArray(gcnew Pitch(pitches[1]     )), newArray((Words::Words^)(gcnew Words::Normal("pizz.")))));
						for each (Rest^ rest in remaining) {
							out[part_cb + 2]->Add(rest);
							out[part_cb + 1]->Add(rest);
							out[part_cb    ]->Add(rest);
						}
					}

					// Ajouter éventuellement des croches de cordes
					addStringNotes(out, pitches, time01, nbEighths);

					// Remplir toutes les parties vides
					for (int part = 1; part <= nbParts; part++) {
						if (out[part]->Count == 0) {
							out[part]->Add(Rest::wholeBarRest(nbEighths * 128));
						}
					}

				}
				void addGraceNotes(array<List<Element^>^, 1>^ out, array<int, 1>^ pitches, int part, int lastPitchIndex, int nbEighths) {
					
					int nbGraceNotes = Rand::chooseIn(newArray(1, 2));

					// Ajouter les appogiatures
					for (int i = 0; i < nbGraceNotes; i++) {
						out[part]->Add(Note::graceNote(
							RhythmFigure::_16th,
							newArray((Pitch^)(gcnew Pitch(pitches[lastPitchIndex - nbGraceNotes + i]))),
							gcnew NoteDecorations(false, i == 0, false)
						));
					}

					// Ajouter la note principale
					out[part]->Add(gcnew Note(
						RhythmFigure::_eighth,
						newArray((Pitch^)(gcnew Pitch(pitches[lastPitchIndex]))),
						gcnew NoteDecorations(Articulation::staccato, false, false, true)
					));

					// Ajouter les silences
					for each (Rest^ rest in Rest::getSum((nbEighths - 1) * 128)) out[part]->Add(rest);

				}
				void addStringNotes(array<List<Element^>^, 1>^ out, array<int, 1>^ pitches, double time01, int nbEighths) {
					
					if (time01 < 0.65) return;

					// Déterminer la probabilité qu'a chaque partie de jouer, en fonction de l'avancement
					double probaPlay = (time01 < 0.85) ? (time01 - 0.65) / (0.85 - 0.65) : 1;

					// Parcourir les parties de violons, d'altos et de violoncelles
					for (int part = part_v1; part < part_cb; part++) {
						// Déterminer si cette partie joue, sinon passer à la suivante
						if (Rand::double01() > probaPlay) continue;
						// Déterminer l'instrument (1 = violon, 2 = alto, 3 = violoncelle)
						int instr; {
							if      (part < part_al ) instr = 1;
							else if (part < part_vcl) instr = 2;
							else                      instr = 3;
						}
						// Déterminer les limites jouables
						int minPossible, maxPossible; {
							if (instr == 1) {
								minPossible = 55;
								maxPossible = 91;
							} else if (instr == 2) {
								minPossible = 48;
								maxPossible = 81;
							} else if (instr == 3) {
								minPossible = 36;
								maxPossible = 69;
							}
						}
						// Trouver les notes jouables
						List<int>^ playablePitches = gcnew List<int>; {
							for each (int pitch in pitches) {
								if (pitch >= minPossible && pitch <= maxPossible) {
									playablePitches->Add(pitch);
								}
							}
							if (playablePitches->Count == 0) continue;
						}
						// Choisir la note
						int pitch = Rand::chooseIn(playablePitches);
						// Choisir le nombre de croches jouées à la fin de la mesure (au moins deux, au plus remplissant la mesure)
						int nbNotes = Rand::integer(2, nbEighths + 1);
						int nbRests = nbEighths - nbNotes;
						// Ajouter les silences et les notes
						for each (Rest^ rest in Rest::getSum(nbRests * 128)) {
							out[part]->Add(rest);
						}
						for (int i = 0; i < nbNotes; i++) {
							NoteDecorations^ deco = gcnew NoteDecorations(Articulation::tenuto, false, i == 0, i == nbNotes - 1);
							Words::Words^ words;
							if      (i == 0          ) words = gcnew Words::Dynamic("p");
							else if (i == nbNotes - 1) words = gcnew Words::Dynamic(time01 < 0.85 ? "f" : "ff");
							else                       words = nullptr;
							if (words == nullptr) out[part]->Add(gcnew Note(RhythmFigure::_eighth, newArray((Pitch^)(gcnew Pitch(pitch))), deco));
							else                  out[part]->Add(gcnew Note(RhythmFigure::_eighth, newArray((Pitch^)(gcnew Pitch(pitch))), deco, words));
						}
					}

				}
		
		};
		ref class BlocksGenerator2 : public BlocksGeneratorForMarkov {
			
			// Variables d'évolution
			private:
				int lastPattern;

			// Implémentation
			public:
				virtual Block^ generate(double time01, int indexInGlobalSeq, int indexOfThisType) override {

					// PROV
					//Block^ bblock = gcnew Block(nbParts);
					//bblock->addBar(gcnew Signature("1/4"));
					//return bblock;
					




					// Initialisations
					if (indexOfThisType == 0) {
						lastPattern = -1;
					}

					// Choisir le pattern, en évitant 'lastPattern', puis actualiser ce dernier
					int pattern; {
						if (indexOfThisType == 0) {
							pattern = 0;
						} else {
							int nbPatterns = 9;
							do {
								double min = time01 * 0.2;
								double max = time01 * 0.5 + 0.5;
								pattern = (int)Math::Floor((min + (max - min) * Rand::double01()) * (nbPatterns - 0.001));
							} while (pattern == lastPattern);
						}
					}
					lastPattern = pattern;

					// Créer le block
					Block^ block = gcnew Block(nbParts);

					// Le piano joue-t-il ?
					bool piano; {
						double time1 = 0.15;
						double time2 = 0.5;
						if      (time01 < time1) piano = false;
						else if (time01 < time2) piano = Rand::double01() < Math::Sqrt((time01 - time1) / (time2 - time1));
						else                     piano = true;
					}

					// Créer les différentes parties, en suivant le pattern choisi
					if      (pattern == 1) fillWithPattern1(block, piano);
					else if (pattern == 2) fillWithPattern2(block, piano);
					else if (pattern == 3) fillWithPattern3(block, piano);
					else if (pattern == 4) fillWithPattern4(block, piano);
					else if (pattern == 5) fillWithPattern5(block, piano);
					else if (pattern == 6) fillWithPattern6(block, piano);
					else if (pattern == 7) fillWithPattern7(block, piano);
					else if (pattern == 8) fillWithPattern8(block, piano);
					else if (pattern == 9) fillWithPattern9(block, piano);
					
					// La main gauche du piano est une transposition de sa main droite
					if (block->nbBars > 0) {
						Group^ rightHand = block[part_pno, 1];
						if (((Object^)rightHand) != nullptr) {
							Group^ leftHand = rightHand->transposition(-11);
							block[part_pno + 1, 1] = leftHand;
						}
					}

					// Retourner
					return block;

				}

			// Fonctions
			private:
				void fillWithPattern1(Block^ block, bool piano) {
					
					block->addBar(qSignature("1/4"));

					// Wood blocks et piano en homorythmie
					block           [part_wbl    , 1] = qGroup("[3:2 e|E5 e|A4 e|A4]");
					block           [part_wbl + 1, 1] = qGroup("[3:2 e|E5 e|F4 e|A4]");
					block           [part_wbl + 2, 1] = qGroup("[3:2 e|E5 e|F4 e|F4]");
					if (piano) block[part_pno    , 1] = qGroup("[3:2 e|C6|D6 e|E5 e|F#5]");

				}
				void fillWithPattern2(Block^ block, bool piano) {
					
					block->addBar(qSignature("1/4"));

					// Wood blocks et piano en homorythmie
					block           [part_wbl    , 1] = qGroup("16th|E5 16th|C5 16th|E5 16th|A4");
					block           [part_wbl + 1, 1] = qGroup("16th|E5 16th|C5 16th|E5 16th|F4");
					block           [part_wbl + 2, 1] = qGroup("16th|C5 16th|A4 16th|C5 16th|F4");
					if (piano) block[part_pno    , 1] = qGroup("16th|C6|D6 16th|A5 16th|C6|D6 16th|E5");

				}
				void fillWithPattern3(Block^ block, bool piano) {
					
					block->addBar(qSignature("3/8"));

					// Wood blocks et piano en homorythmie
					block           [part_wbl    , 1] = qGroup("16th|F4 16th|A4 16th|C5 16th|E5 e|A4");
					block           [part_wbl + 1, 1] = qGroup("16th|F4 16th|C5 16th|A4 16th|E5 e|C5");
					block           [part_wbl + 2, 1] = qGroup("16th|F4 16th|F4 16th|C5 16th|C5 e|A4");
					if (piano) block[part_pno    , 1] = qGroup("16th|E5 16th|F#5 16th|A5 16th|C6 e|E5|F#5");

				}
				void fillWithPattern4(Block^ block, bool piano) {
					
					block->addBar(qSignature("3/8"));

					// Wood blocks et piano en homorythmie
					block           [part_wbl    , 1] = qGroup("16th|F4 16th|C5 16th 16th|A4 16th|E5 16th");
					block           [part_wbl + 1, 1] = qGroup("16th|F4 16th|C5 16th 16th|A4 16th|E5 16th");
					block           [part_wbl + 2, 1] = qGroup("16th|F4 16th|C5 16th 16th|A4 16th|E5 16th");
					if (piano) block[part_pno    , 1] = qGroup("16th|E5 16th|A5 16th 16th|F#5 16th|C6|D6 16th");

				}
				void fillWithPattern5(Block^ block, bool piano) {
					
					block->addBar(qSignature("5/8"));

					// Wood blocks et piano en homorythmie
					block           [part_wbl    , 1] = qGroup("[e:3:2 q e|E5] e 16th|E5 16th|C5 16th|E5 16th|A4");
					block           [part_wbl + 1, 1] = qGroup("[e:3:2 q e|E5] e 16th|E5 16th|C5 16th|E5 16th|F4");
					block           [part_wbl + 2, 1] = qGroup("[e:3:2 q e|E5] e 16th|C5 16th|A4 16th|C5 16th|F4");
					if (piano) block[part_pno    , 1] = qGroup("[e:3:2 q e|C6|D6] e 16th|C6|D6 16th|A5 16th|C6|D6 16th|E5");

				}
				void fillWithPattern6(Block^ block, bool piano) {
					
					block->addBar(qSignature("1/4"));

					// Wood blocks en homorythmie
					block           [part_wbl    , 1] = qGroup("[5:4 16th|A4 16th|C5 16th|C5 16th|E5 16th|E5]");
					block           [part_wbl + 1, 1] = qGroup("[5:4 16th|F4 16th|A4 16th|A4 16th|C5 16th|C5]");
					block           [part_wbl + 2, 1] = qGroup("[5:4 16th|F4 16th|F4 16th|A4 16th|A4 16th|C5]");

				}
				void fillWithPattern7(Block^ block, bool piano) {
					
					block->addBar(qSignature("1/4"));

					// Wood blocks en homorythmie
					block           [part_wbl    , 1] = qGroup("[6:4 16th|A4 16th|C5 16th|C5 16th|A4 16th 16th|E5]");
					block           [part_wbl + 1, 1] = qGroup("[6:4 16th|A4 16th|C5 16th|E5 16th|F4 16th 16th|E5]");
					block           [part_wbl + 2, 1] = qGroup("[6:4 16th|F4 16th|C5 16th|A4 16th|A4 16th 16th|C5]");

				}
				void fillWithPattern8(Block^ block, bool piano) {
					
					block->addBar(qSignature("1/4"));

					// Wood blocks et piano en homorythmie
					block           [part_wbl    , 1] = qGroup("16th|C5 16th|A4 e|C5 16th|E5 16th|A4 16th|C5 16th|F4 16th 16th|F4 16th|C5 16th");
					block           [part_wbl + 1, 1] = qGroup("16th|C5 16th|A4 e|C5 16th|C5 16th|A4 16th|C5 16th|F4 16th 16th|F4 16th|C5 16th");
					block           [part_wbl + 2, 1] = qGroup("16th|E5 16th|A4 e|C5 16th|C5 16th|C5 16th|A4 16th|F4 16th 16th|A4 16th|E5 16th");
					if (piano) block[part_pno    , 1] = qGroup("16th|A5|B5 16th|F#5 e|A5|B5 16th|C6|D6 16th|F#5 16th|A5 16th|E5 16th 16th|E5 16th|A5|B5 16th");

				}
				void fillWithPattern9(Block^ block, bool piano) {
					
					block->addBar(qSignature("1/4"));

					// Wood blocks en homorythmie
					block           [part_wbl    , 1] = qGroup("[3:2 16th|E5 16th|E5 16th|E5] e|C5 16th|F4 16th|F4 16th|F4 16th|C5 16th 16th|A4");
					block           [part_wbl + 1, 1] = qGroup("[3:2 16th|E5 16th|C5 16th|E5] e|A4 16th|F4 16th|F4 16th|A4 16th|C5 16th 16th|A4");
					block           [part_wbl + 2, 1] = qGroup("[3:2 16th|C5 16th|C5 16th|C5] e|A4 16th|F4 16th|C5 16th|A4 16th|E5 16th 16th|C5");

				}
		
		};
		ref class BlocksGenerator3 : public BlocksGeneratorForMarkov {

			// Implémentation
			public:
				virtual Block^ generate(double time01, int indexInGlobalSeq, int indexOfThisType) override {

					// PROV
					//Block^ bblock = gcnew Block(nbParts);
					//bblock->addBar(gcnew Signature("1/4"));
					//return bblock;





					// Choisir le pattern
					int pattern; {
						if (time01 < 0.6) pattern = Rand::chooseIn(newArray(1, 2));
						else              pattern = Rand::chooseIn(newArray(1, 2, 3));
					}

					// Créer le block
					Block^ block = gcnew Block(nbParts);

					// Créer les différentes parties, en suivant le pattern choisi
					if      (pattern == 1) fillWithPattern1(block);
					else if (pattern == 2) fillWithPattern2(block);
					else if (pattern == 3) fillWithPattern3(block);
					
					// Si besoin, ajouter les clarinettes à l'octave et les cordes
					bool clarinets = (indexOfThisType % 2 == 1) || indexOfThisType >= 4;
					bool strings   = (indexOfThisType % 4 >= 2) || indexOfThisType >= 4;
					if (clarinets) {
						block[part_cl    , 1] = copyWithSlurAndTransp(block[part_tpt    , 1], +12);
						block[part_cl + 1, 1] = copyWithSlurAndTransp(block[part_tpt + 1, 1], +12);
						block[part_cl + 2, 1] = copyWithSlurAndTransp(block[part_tpt + 2, 1], +12);
					}
					if (strings) {
						block[part_v1    , 1] = copyWithSlurAndTransp(block[part_tpt    , 1], +12);
						block[part_v1 + 1, 1] = copyWithSlurAndTransp(block[part_tpt    , 1], +12);
						block[part_v1 + 2, 1] = copyWithSlurAndTransp(block[part_tpt + 1, 1], +12);
						block[part_v1 + 3, 1] = copyWithSlurAndTransp(block[part_tpt + 1, 1], +12);
						block[part_v1 + 4, 1] = copyWithSlurAndTransp(block[part_tpt + 1, 1], +12);
						block[part_v1 + 5, 1] = copyWithSlurAndTransp(block[part_tpt + 2, 1], +12);
						block[part_v1 + 6, 1] = copyWithSlurAndTransp(block[part_tpt + 2, 1], +12);
						block[part_v1 + 7, 1] = copyWithSlurAndTransp(block[part_tpt + 2, 1], +12);
						block[part_v2    , 1] = copyWithSlurAndTransp(block[part_tpt    , 1],   0);
						block[part_v2 + 1, 1] = copyWithSlurAndTransp(block[part_tpt    , 1],   0);
						block[part_v2 + 2, 1] = copyWithSlurAndTransp(block[part_tpt    , 1],   0);
						block[part_v2 + 3, 1] = copyWithSlurAndTransp(block[part_tpt + 1, 1],   0);
						block[part_v2 + 4, 1] = copyWithSlurAndTransp(block[part_tpt + 1, 1],   0);
						block[part_v2 + 5, 1] = copyWithSlurAndTransp(block[part_tpt + 2, 1],   0);
						block[part_v2 + 6, 1] = copyWithSlurAndTransp(block[part_tpt + 2, 1],   0);
						block[part_al    , 1] = copyWithSlurAndTransp(block[part_tpt    , 1],   0);
						block[part_al + 1, 1] = copyWithSlurAndTransp(block[part_tpt    , 1],   0);
						block[part_al + 2, 1] = copyWithSlurAndTransp(block[part_tpt + 1, 1],   0);
						block[part_al + 3, 1] = copyWithSlurAndTransp(block[part_tpt + 1, 1],   0);
						block[part_al + 4, 1] = copyWithSlurAndTransp(block[part_tpt + 2, 1],   0);
						block[part_al + 5, 1] = copyWithSlurAndTransp(block[part_tpt + 2, 1],   0);
					}

					// Retourner
					return block;

				}

			// Fonctions
			private:
				void fillWithPattern1(Block^ block) {
					
					block->addBar(qSignature("2/4"));

					// Trompettes en homorythmie
					block[part_tpt    , 1] = qGroup("16th'|E4 16th'|G4 16th'|A#4 16th'|B4 [3:2 e'|C5 e'|D#5 e]");
					block[part_tpt + 1, 1] = qGroup("16th'|D#4 16th'|E4 16th'|G#4 16th'|A4 [3:2 e'|B4 e'|C5 e]");
					block[part_tpt + 2, 1] = qGroup("16th'|C4 16th'|Eb4 16th'|G4 16th'|G#4 [3:2 e'|A#4 e'|B4 e]");

					// Trombones en homorythmie
					block[part_tbn    , 1] = qGroup("q-|B3<.gliss.> e^|E4 e");
					block[part_tbn + 1, 1] = qGroup("q-|G3<.gliss.> e^|C4 e");
					block[part_tbn + 2, 1] = qGroup("q-|E3<.gliss.> e^|Bb3 e");

					// Piano (accord de fin)
					block[part_pno + 1, 1] = qGroup("q e'@|B4|C5|D#5 e");
					block[part_pno    , 1] = block[part_pno + 1, 1]->transposition(+12);

				}
				void fillWithPattern2(Block^ block) {
					
					block->addBar(qSignature("3/8"));

					// Trompettes en homorythmie
					block[part_tpt    , 1] = qGroup("16th'|G4 16th'|G#4 16th'|C5 16th'|D#5 e'|E5");
					block[part_tpt + 1, 1] = qGroup("16th'|E4 16th'|G4 16th'|Ab4 16th'|Bb4 e'|C5");
					block[part_tpt + 2, 1] = qGroup("16th'|C4 16th'|D#4 16th'|E4 16th'|G4 e'|Bb4");

					// Trombones en homorythmie
					block[part_tbn    , 1] = qGroup("q-|B3<.gliss.> e^|D#4");
					block[part_tbn + 1, 1] = qGroup("q-|G#3<.gliss.> e^|B3");
					block[part_tbn + 2, 1] = qGroup("q-|E3<.gliss.> e^|A3");

					// Piano (accord de fin)
					block[part_pno + 1, 1] = qGroup("q e'@|Bb4|C5|E5");
					block[part_pno    , 1] = block[part_pno + 1, 1]->transposition(+12);

				}
				void fillWithPattern3(Block^ block) {
					
					block->addBar(qSignature("3/8"));

					// Trompettes en homorythmie
					block[part_tpt    , 1] = qGroup("[5:4 16th-(|G#4 16th|A4 16th|C5 16th|D#5 16th|E5] e)'|G5");
					block[part_tpt + 1, 1] = qGroup("[5:4 16th-(|G4 16th|Ab4 16th|Bb4 16th|C5 16th|D#5] e)'|E5");
					block[part_tpt + 2, 1] = qGroup("[5:4 16th-(|E4 16th|G4 16th|G#4 16th|B4 16th|C5] e)'|Eb5");

					// Trombones en homorythmie
					block[part_tbn    , 1] = qGroup("q-|E4<.gliss.> e^|C4");
					block[part_tbn + 1, 1] = qGroup("q-|C4<.gliss.> e^|Ab3");
					block[part_tbn + 2, 1] = qGroup("q-|Bb3<.gliss.> e^|E3");

					// Piano (accord de fin)
					block[part_pno + 1, 1] = qGroup("q e'@|D#5|E5|G5 e");
					block[part_pno    , 1] = block[part_pno + 1, 1]->transposition(+12);

				}
			private:
				Group^ copyWithSlurAndTransp(Group^ group, int transp) {
					
					// Transposer
					group = group->transposition(transp);

					// Changer le premier élément pour porter l'indication "slur"
					array<Element^, 1>^ elements = group->elements;
					{
						Element^ elem = elements[0];
						Words::Words^ words = gcnew Words::Italic("slur");
						if (elem->isRest()) {
							Rest^ elem_rest = (Rest^)elem;
							elements[0] = gcnew Rest(elem_rest->rhythm, words);
						} else if (elem->isNote()) {
							Note^ elem_note = (Note^)elem;
							elements[0] = gcnew Note(elem_note->rhythm, elem_note->pitches, elem_note->decorations, words);
						} else {
							Tuplet^ elem_tuplet = (Tuplet^)elem;
							array<NoteOrRest^, 1>^ subElements = elem_tuplet->elements;
							{
								Element^ subElem = subElements[0];
								if (subElem->isRest()) {
									Rest^ subElem_rest = (Rest^)subElem;
									subElements[0] = gcnew Rest(subElem_rest->rhythm, words);
								} else if (subElem->isNote()) {
									Note^ subElem_note = (Note^)subElem;
									subElements[0] = gcnew Note(subElem_note->rhythm, subElem_note->pitches, subElem_note->decorations, words);
								}
							}
							elements[0] = gcnew Tuplet(elem_tuplet->ratio, subElements);
						}
					}

					// Retourner
					return gcnew Group(elements);

				}

		};
		ref class BlocksGenerator4 : public BlocksGeneratorForMarkov {

			// Implémentation
			public:
				virtual Block^ generate(double time01, int indexInGlobalSeq, int indexOfThisType) override {

					// PROV
					//Block^ bblock = gcnew Block(nbParts);
					//bblock->addBar(gcnew Signature("1/4"));
					//return bblock;





					// Fixer le nombre de Thue-notes
					int nbNotes; {
						if (indexOfThisType == 0) {
							nbNotes = 5;
						} else {
							double min, max; bool distort; {
								if (time01 < 0.2) {
									double time = time01 / 0.2;
									min = 3 - time;
									max = 5 + time;
									distort = false;
								} else {
									double time = Math::Pow((time01 - 0.2) / (1 - 0.2), 1.5);
									min = 2;
									max = 6 + 11 * time;
									distort = true;
								}
								min -= 0.49;
								max += 0.49;
							}
							double rand = Rand::double01();
							if (distort) rand = Math::Sqrt(rand);
							nbNotes = (int)Math::Round(min + (max - min) * Rand::double01());
						}
					}

					// Calculer la suite de Thue-Morse (modifiée si besoin), et en déduire le nombre total de croches
					array<bool, 1>^ thue = Seq::thueMorse(nbNotes); {
						double probaTransfToEighth = (time01 - 0.85) / (1 - 0.85) * 0.5;
						for (int i = 0; i < nbNotes; i++) {
							if (thue[i] && Rand::double01() < probaTransfToEighth) {
								thue[i] = false;
							}
						}
					}
					int nbEighths; {
						nbEighths = 0;
						for each (bool thueElement in thue) {
							nbEighths += thueElement ? 2 : 1;
						}
					}

					// Calculer le contenu harmonique de chaque Thue-note
					array<array<int, 1>^, 1>^ chords = getChords(nbNotes, time01, indexOfThisType);

					// Créer le block avec une unique mesure
					Block^ block = gcnew Block(nbParts);
					block->addBar(gcnew Signature(nbEighths * 128, nullptr));

					// Ajouter tous les éléments de chaque partie
					addElements(block, thue, chords, nbNotes, nbEighths, time01);

					// Retourner
					return block;

				}
		
		// Fonctions
		private:
			static array<int, 1>^ centralNotes = gcnew array<int, 1> { 67, 72, 62, 68, 68, 65, 71, 64, 73, 62, 64, 70, 67, 69, 66, 71, 63, 61 };
			array<array<int, 1>^, 1>^ getChords(int nbNotes, double time01, int indexOfThisType) {
				
				array<array<int, 1>^, 1>^ ans = gcnew array<array<int, 1>^, 1>(nbNotes);

				for (int i = 0; i < nbNotes; i++) {
					int centralNote = centralNotes[i % (centralNotes->Length)];
					int spread; {
						double minSpread = 3; minSpread -= 0.49;
						double maxSpread = 5; maxSpread += 0.49;
						spread = (int)Math::Round(minSpread + (maxSpread - minSpread) * time01);
						if (indexOfThisType != 0 && spread == 3 && Rand::double01() < 0.35) spread = 2;
					}
					int nbPitches = 1 + 2 * spread;
					array<int, 1>^ chord = gcnew array<int, 1>(nbPitches);
					for (int pos = -spread; pos <= spread; pos++) {
						chord[pos + spread] = centralNote + pos;
					}
					ans[i] = chord;
				}

				return ans;

			}
			void addElements(Block^ block, array<bool, 1>^ thue, array<array<int, 1>^, 1>^ chords, int nbNotes, int nbEighths, double time01) {
				
				// Créer un tableau de tous les éléments à ajouter (durant chacun une croche) dans chaque partie
				array<List<Element^>^, 1>^ elementsToAdd = gcnew array<List<Element^>^, 1>(nbParts + 1);
				for (int part = 1; part <= nbParts; part++) elementsToAdd[part] = gcnew List<Element^>;

				// Ajouter chaque note dans le tableau
				for (int i = 0; i < nbNotes; i++) {
					if (thue[i]) {
						// Valeur longue (croche puis demi-soupir)
						addChord(elementsToAdd, chords[i], time01);
						addBum(elementsToAdd, time01);
					} else {
						// Valeur courte (croche)
						addChord(elementsToAdd, chords[i], time01);
					}
				}

				// Tout ajouter au block
				for (int part = 1; part <= nbParts; part++) {
					block[part, 1] = gcnew Group(elementsToAdd[part]);
				}

			}
			void addChord(array<List<Element^>^, 1>^ out, array<int, 1>^ chord, double time01) {
				
				bool oneOctave = time01 < 0.88;

				// Instruments concernés par la répartition aléatoire des notes
				array<int, 1> ^partsHi, ^partsLo; {
					if (oneOctave) {
						// Accord joué sur une seule octave
						partsHi = gcnew array<int, 1> {
							part_fl, part_fl + 1, part_fl + 2,
							part_hb, part_hb + 1, part_hb + 2,
							part_cl, part_cl + 1, part_cl + 2,
							part_bsn, part_bsn + 1, part_bsn + 2,
							part_cor, part_cor + 1, part_cor + 2, part_cor + 3,
							part_tpt, part_tpt + 1, part_tpt + 2,
							part_v1, part_v1 + 1, part_v1 + 2, part_v1 + 3, part_v1 + 4, part_v1 + 5, part_v1 + 6, part_v1 + 7,
							part_v2, part_v2 + 1, part_v2 + 2, part_v2 + 3, part_v2 + 4, part_v2 + 5, part_v2 + 6,
							part_al, part_al + 1, part_al + 2, part_al + 3, part_al + 4, part_al + 5
						};
						partsLo = gcnew array<int, 1> { };
					} else {
						// Accord joué sur deux octaves (avec l'octave inférieure)
						partsHi = gcnew array<int, 1> {
							part_fl, part_fl + 1, part_fl + 2,
							part_hb, part_hb + 1, part_hb + 2,
							part_cl, part_cl + 1, part_cl + 2,
							part_tpt, part_tpt + 1, part_tpt + 2,
							part_v1, part_v1 + 1, part_v1 + 2, part_v1 + 3, part_v1 + 4, part_v1 + 5, part_v1 + 6, part_v1 + 7,
							part_v2, part_v2 + 1, part_v2 + 2, part_v2 + 3, part_v2 + 4, part_v2 + 5, part_v2 + 6
						};
						partsLo = gcnew array<int, 1> {
							part_bsn, part_bsn + 1, part_bsn + 2,
							part_cor, part_cor + 1, part_cor + 2, part_cor + 3,
							part_tbn, part_tbn + 1, part_tbn + 2,
							part_al, part_al + 1, part_al + 2, part_al + 3, part_al + 4, part_al + 5,
							part_vcl, part_vcl + 1, part_vcl + 2, part_vcl + 3, part_vcl + 4
						};
					}
				}

				// Accord aux trois registres possibles
				array<int, 1> ^chord_lo, ^chord_me, ^chord_hi;
				int nbPitchesInChord;
				{
					nbPitchesInChord = chord->Length;
					chord_lo = gcnew array<int, 1>(nbPitchesInChord);
					chord_me = gcnew array<int, 1>(nbPitchesInChord);
					chord_hi = gcnew array<int, 1>(nbPitchesInChord);
					for (int i = 0; i < nbPitchesInChord; i++) {
						chord_lo[i] = chord[i] - 12;
						chord_me[i] = chord[i]     ;
						chord_hi[i] = chord[i] + 12;
					}
					Array::Sort(chord_lo);
					Array::Sort(chord_me);
					Array::Sort(chord_hi);
				}

				// Ajout des notes choisies aléatoirement
				for (int reg = 0; reg <= 1; reg++) {
					array<int, 1>^ parts = (reg == 0) ? partsHi : partsLo;
					for each (int part in parts) {
						// Déterminer le registre accessible
						int min, max; {
							if      (part == part_fl                           ) { min = 74; max = 108; }
							else if (part >= part_fl + 1 && part <= part_fl + 2) { min = 60; max =  98; }
							else if (part >= part_hb && part <= part_hb + 2    ) { min = 58; max =  82; }
							else if (part >= part_cl && part <= part_cl + 2    ) { min = 50; max =  90; }
							else if (part >= part_bsn && part <= part_bsn + 2  ) { min = 34; max =  67; }
							else if (part >= part_cor && part <= part_cor + 3  ) { min = 41; max =  58; }
							else if (part >= part_tpt && part <= part_tpt + 2  ) { min = 59; max =  77; }
							else if (part >= part_tbn && part <= part_tbn + 2  ) { min = 43; max =  60; }
							else if (part == part_tub                          ) { min = 36; max =  54; }
							else if (part >= part_v1 && part <= part_v2 + 6    ) { min = 55; max =  88; }
							else if (part >= part_al && part <= part_al + 5    ) { min = 48; max =  81; }
							else if (part >= part_vcl && part <= part_vcl + 4  ) { min = 36; max =  65; }
							else                                                 throw gcnew Exception;
						}
						// Déterminer les notes possibles
						List<int>^ possiblePitches = gcnew List<int>; {
							// Ajouter préférentiellement dans les octaves modifiées
							if (!oneOctave) {
								for each (int pitch in chord_lo) if (pitch >= min && pitch <= max) possiblePitches->Add(pitch);
								for each (int pitch in chord_hi) if (pitch >= min && pitch <= max) possiblePitches->Add(pitch);
							}
							// Ajouter dans l'octave normale si aucune ne convenait
							if (possiblePitches->Count == 0) {
								for each (int pitch in chord_me) if (pitch >= min && pitch <= max) possiblePitches->Add(pitch);
							}
						}
						// Choisir une note pour cette partie, ou -1
						int pitch_or_m1; {
							if (possiblePitches->Count == 0) {
								pitch_or_m1 = -1;
							} else {
								pitch_or_m1 = Rand::chooseIn(possiblePitches);
							}
						}
						// Ajouter
						if (pitch_or_m1 == -1) {
							out[part]->Add(qElement("e"));
						} else {
							Pitch^ pitch = gcnew Pitch(pitch_or_m1);
							NoteDecorations^ deco = gcnew NoteDecorations(gcnew array<Articulation^, 1> { Articulation::staccato, Articulation::accent });
							out[part]->Add(gcnew Note(RhythmFigure::_eighth, pitch, deco));
						}
					}
				}

				// Ajout du piano
				{
					Pitch^ pitchR = gcnew Pitch(chord_me[nbPitchesInChord - 1]);
					Pitch^ pitchL = gcnew Pitch(chord_me[0                   ]);
					NoteDecorations^ deco = gcnew NoteDecorations(gcnew array<Articulation^, 1> { Articulation::staccato, Articulation::accent });
					out[part_pno    ]->Add(gcnew Note(RhythmFigure::_eighth, pitchR, deco));
					out[part_pno + 1]->Add(gcnew Note(RhythmFigure::_eighth, pitchL, deco));
				}

				// Ajout de demi-soupirs dans les parties où l'on n'a rien ajouté
				for (int part = 1; part <= nbParts; part++) {
					if (!contains(partsHi, part) && !contains(partsLo, part) && part != part_pno && part != part_pno + 1) {
						out[part]->Add(qElement("e"));
					}
				}

			}
			void addBum(array<List<Element^>^, 1>^ out, double time01) {
				
				for (int part = 1; part <= nbParts; part++) {
					if (part == part_bum) {
						// Coup de grosse caisse
						out[part]->Add(qElement("e|B4<!fff>"));
					} else if (time01 > 0.85 && (part >= part_cb && part <= part_cb + 2)) {
						// Coup de contrebasses
						out[part]->Add(qElement("e|G1<:bum>"));
					} else {
						// Silence
						out[part]->Add(qElement("e"));
					}
				}

			}

		};

	// Fonctions
	private:
		void fillWoodBlocks23(Block^ block) {
			
			// Déterminer la taille du block, en croches
			int nbEighths = block->getDuration_1024th() / 128;

			// Générer Thue de façon très large (et en commençant après 16 valeurs)
			array<bool, 1>^ thue; {
				int genNb = 16 + nbEighths;
				thue = Seq::thueMorse(genNb);
				for (int i = 16; i < genNb; i++) {
					thue[i - 16] = thue[i];
				}
				thue = reduceSize(thue, genNb - 16);
			}

			// En déduire un tableau de notes jouées et non jouées, sur la bonne durée
			array<bool, 1>^ played = gcnew array<bool, 1>(nbEighths); {
				int cpt = 0;
				for each (bool thueElement in thue) {
					if (thueElement) {
						played[cpt++] = true ; if (cpt == nbEighths) break;
						played[cpt++] = false; if (cpt == nbEighths) break;
					} else {
						played[cpt++] = true ; if (cpt == nbEighths) break;
					}
				}
			}

			// Placer les accents toutes les 12 croches
			array<bool, 1>^ accent = gcnew array<bool, 1>(nbEighths); {
				for (int i = 0; i < nbEighths; i++) {
					accent[i] = i % 12 == 0;
				}
			}

			// Déterminer quelle note est jouée quand (la note 1 est la plus grave, la note 4 est la plus aiguë)
			array<bool, 1> ^playedNote1, ^playedNote2, ^playedNote3, ^playedNote4; {
				playedNote1 = gcnew array<bool, 1>(nbEighths);
				playedNote2 = gcnew array<bool, 1>(nbEighths);
				playedNote3 = gcnew array<bool, 1>(nbEighths);
				playedNote4 = gcnew array<bool, 1>(nbEighths);
				for (int i = 0; i < nbEighths; i++) {
					int mode; // 1 = xxxx, 2 = x_x_, 3 = x____, 4 = __xx alternés, 5 = x_x_ alternés
					{
						double time = ((double)i) / (nbEighths - 1);
						if      (time < 1. / 7) mode = 1;
						else if (time < 2. / 7) mode = 2;
						else if (time < 3. / 7) mode = 3;
						else if (time < 4. / 7) mode = 4;
						else if (time < 5. / 7) mode = 5;
						else if (time < 6. / 7) mode = 2;
						else                    mode = 1;
					}
					if (played[i]) {
						if (mode == 1) {
							// xxxx
							playedNote1[i] = true;
							playedNote2[i] = true;
							playedNote3[i] = true;
							playedNote4[i] = true;
						} else if (mode == 2) {
							// x_x_
							playedNote1[i] = true;
							playedNote2[i] = false;
							playedNote3[i] = true;
							playedNote4[i] = false;
						} else if (mode == 3) {
							// x____
							playedNote1[i] = true;
							playedNote2[i] = false;
							playedNote3[i] = false;
							playedNote4[i] = false;
						} else if (mode == 4) {
							// ¨_xx alternés
							playedNote1[i] = i % 5 == 2;
							playedNote2[i] = false;
							playedNote3[i] = i % 2 == 0;
							playedNote4[i] = i % 2 == 1;
						} else if (mode == 5) {
							// x_x¨ alternés
							playedNote1[i] = i % 2 == 0;
							playedNote2[i] = false;
							playedNote3[i] = i % 2 == 1;
							playedNote4[i] = i % 5 == 2;
						}
					} else {
						playedNote1[i] = false;
						playedNote2[i] = false;
						playedNote3[i] = false;
						playedNote4[i] = false;
					}
				}
			}

			// Remplir
			fillWoodBlocks(block, part_wbl + 1, nbEighths, playedNote3, 72, playedNote4, 76, accent);
			fillWoodBlocks(block, part_wbl + 2, nbEighths, playedNote1, 65, playedNote2, 69, accent);

		}
		void fillWoodBlocks(Block^ block, int part, int nbEighths, array<bool, 1>^ played1, int pitch1, array<bool, 1>^ played2, int pitch2, array<bool, 1>^ accent) {
			
			int nbBars = block->nbBars;

			// Etablir un tableau des éléments joués, par mesure
			array<array<Element^, 1>^, 1>^ elementsByBar = gcnew array<array<Element^, 1>^, 1>(nbBars); {
				for (int bar = 1; bar <= nbBars; bar++) {
					Group^ group = block[part, bar];
					if (group != nullptr) {
						elementsByBar[bar - 1] = group->elements;
					} else {
						Signature^ timeSignature = block->timeSignature[bar];
						int barDur_1024th = timeSignature->duration_1024th;
						if (barDur_1024th % 128 != 0) throw gcnew Exception; 
						int nbEighthsInBar = barDur_1024th / 128;
						elementsByBar[bar - 1] = fill((Element^)(gcnew Rest(RhythmFigure::_eighth)), nbEighthsInBar);
					}
				}
			}

			// Remplacer les croches libres dans ce tableau
			int cpt_eighths = 0;
			for (int bar = 0; bar < nbBars; bar++) {
				Group^ group = gcnew Group(elementsByBar[bar]);
				int nbEighthsInThisBar = group->duration_1024th / 128;
				array<bool, 1>^ played1_inThisBar = gcnew array<bool, 1>(nbEighthsInThisBar);
				array<bool, 1>^ played2_inThisBar = gcnew array<bool, 1>(nbEighthsInThisBar);
				array<bool, 1>^ accents_inThisBar = gcnew array<bool, 1>(nbEighthsInThisBar);
				for (int i = 0; i < nbEighthsInThisBar; i++) {
					played1_inThisBar[i] = played1[i + cpt_eighths];
					played2_inThisBar[i] = played2[i + cpt_eighths];
					accents_inThisBar[i] = accent [i + cpt_eighths];
				}
				elementsByBar[bar] = replaceEighths(elementsByBar[bar], nbEighthsInThisBar, played1_inThisBar, pitch1, played2_inThisBar, pitch2, accents_inThisBar);
				cpt_eighths += nbEighthsInThisBar;
			}

			// Réinjecter dans le block
			for (int bar = 1; bar <= nbBars; bar++) {
				block[part, bar] = gcnew Group(elementsByBar[bar - 1]);
			}

		}
		array<Element^, 1>^ replaceEighths(array<Element^, 1>^ elements, int nbEighths, array<bool, 1>^ played1, int pitch1, array<bool, 1>^ played2, int pitch2, array<bool, 1>^ accent) {
			
			List<Element^>^ ans = gcnew List<Element^>;

			// Parcourir les éléments
			int cpt_dur = 0;
			for each (Element^ element in elements) {
				int thisElementDuration = element->duration_1024th;
				if (cpt_dur % 128 == 0) {
					int eighthIndex = cpt_dur / 128;
					if (element->isRest() && thisElementDuration % 128 == 0) {
						int nbEighthsInThisRest = thisElementDuration / 128;
						for (int i = 0; i < nbEighthsInThisRest; i++) {
							bool play1 = played1[eighthIndex + i];
							bool play2 = played2[eighthIndex + i];
							bool acc   = accent[eighthIndex + i];
							if (acc) { play1 = true; play2 = true; }
							if (play1 || play2) {
								List<Pitch^>^ pitchesForNote = gcnew List<Pitch^>;
								if (play1) pitchesForNote->Add(gcnew Pitch(pitch1));
								if (play2) pitchesForNote->Add(gcnew Pitch(pitch2));
								array<Articulation^, 1>^ articulations = acc ? newArray(Articulation::accent) : gcnew array<Articulation^, 1> { };
								ans->Add(gcnew Note(
									RhythmFigure::_eighth,
									pitchesForNote->ToArray(),
									gcnew NoteDecorations(articulations)
								));
							} else {
								ans->Add(gcnew Rest(RhythmFigure::_eighth));
							}
						}
					} else {
						ans->Add(element);
					}
				} else {
					ans->Add(element);
				}
				cpt_dur += thisElementDuration;
			}

			// Retourner
			return ans->ToArray();

		}

};
