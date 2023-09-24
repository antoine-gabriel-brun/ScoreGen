#pragma region head

	#pragma once

	#include "OF_FractalSeq.h"
	#include "OF_Descente.h"
	#include "OF_TestContraintes.h"
	#include "OF_ThueInRythm.h"
	#include "OF_SquareFreeSequenceInRhythm.h"

#pragma endregion

// Créations 1
public ref class G_FractalSeq : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Suite fractale";

		}
		virtual String^ description() override {
			
			return "Morceau monodique consistant en une suite fractale paramétrable réalisée sur un mode.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			// Récupérer les options
			array<int, 1>^ pitches;
			array<int, 1>^ durations;
			int nbNotes;
			{
				// Montrer la boîte de dialogue
				ScoreGen::OF_FractalSeq^ dialog = gcnew ScoreGen::OF_FractalSeq(name(), description());
				dialog->ShowDialog();
				// Récupérer les valeurs des options
				array<Object^, 1>^ options = dialog->getOptions();
				if (options == nullptr) return nullptr;
				// Convertir
				pitches   = (array<int, 1>^)(options[0]);
				durations = (array<int, 1>^)(options[1]);
				nbNotes = pitches->Length;
			}

			// Convertir chaque durée en une série de notes liées entre elles. Exemple : 1728 = 1024+512+128+64 = ronde pointée liée à croche pointée.
			array<array<Rhythm^, 1>^, 1>^ rhythms; {
				rhythms = gcnew array<array<Rhythm^, 1>^, 1>(nbNotes);
				for (int i = 0; i < nbNotes; i++) {
					rhythms[i] = Rhythm::getSum(durations[i]);
				}
			}

			// Calculer la durée totale
			int totalDuration = 0; for each (int duration in durations) totalDuration += duration;

			// Créer le bloc, avec une partie, et une mesure contenant la durée totale
			Block^ block = gcnew Block;
			block->addPart();
			block->addBar(gcnew Signature(totalDuration, nullptr));

			// Ajouter les notes
			List<Element^>^ elements = gcnew List<Element^>;
			for (int i = 0; i < nbNotes; i++) {
				array<Rhythm^, 1>^ rhythmicSum = rhythms[i];
				int rhythmicSumLen = rhythmicSum->Length;
				Pitch^ pitch = gcnew Pitch(pitches[i]);
				for (int j = 0; j < rhythmicSumLen; j++) {
					elements->Add(gcnew Note(rhythmicSum[j], pitch, gcnew NoteDecorations(j < rhythmicSumLen - 1, false, false)));
				}
			}
			block[1, 1] = gcnew Group(elements);

			// Retourner
			return block;

		}

};
public ref class G_RFTW_Descente : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Requiem for the World – Descente";

		}
		virtual String^ description() override {
			
			return "Longue glissade exponentielle écrite en demi-tons pour quatuor.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			// Récupérer les options
			array<List<ScoreGen::OF_Descente::NNote^>^, 1>^ notes; {
				// Montrer la boîte de dialogue
				auto dialog = gcnew ScoreGen::OF_Descente(name(), description());
				dialog->ShowDialog();
				// Récupérer les valeurs des options
				array<Object^, 1>^ options = dialog->getOptions();
				if (options == nullptr) return nullptr;
				// Convertir
				notes = (array<List<ScoreGen::OF_Descente::NNote^>^, 1>^)(options[0]);
			}

			// Récupérer l'instant de la dernière note et le nombre de positions rythmiques
			int lastMoment = (int)Math::Round(Math::Max(Math::Max(last(notes[0]), last(notes[1])), Math::Max(last(notes[2]), last(notes[3]))));
			int subdPerTimeUnit = 12;
			int timeUnitsPerBar = 4;
			int subdPerBar = timeUnitsPerBar * subdPerTimeUnit;
			int nbPos = subdPerTimeUnit * (lastMoment - 1) + 1;
			nbPos = modCeiling(nbPos, subdPerBar) + subdPerBar;
			int nbBars = nbPos / subdPerBar;

			// Placer les notes dans un grand tableau
			array<int, 2>^ notesPos = gcnew array<int, 2>(4, nbPos);
			for (int i = 0; i < 4; i++) {
				// Initialiser la ligne à -1
				for (int j = 0; j < nbPos; j++) notesPos[i, j] = -1;
				// Poser les notes
				for each (ScoreGen::OF_Descente::NNote^ note in notes[i]) {
					// Calculer la position
					int pos = (int)Math::Round(note->moment * 12);
					// Corriger pour ne pas tomber sur les 12-olets n° 1, 2, 5, 7, 10, 11
					int posMod = pos % 12;
					if      (posMod == 1 || posMod == 7 || posMod == 10) pos--;
					else if (posMod == 2 || posMod == 5 || posMod == 11) pos++;
					// Placer
					notesPos[i, pos] = note->pitch;
				}
			}

			// Créer le bloc, avec quatre parties et 'nbBars' mesures en C
			Block^ block = gcnew Block;
			block->addParts(4);
			block->addBars(nbBars, "C");
			
			// Ajouter les mesures
			for (int part = 1; part <= 4; part++) {
				for (int bar = 1; bar <= nbBars; bar++) {
					List<Element^>^ elements = gcnew List<Element^>;
					for (int beat = 0; beat < timeUnitsPerBar; beat++) {
						array<int, 1>^ twelveValues = gcnew array<int, 1>(12);
						for (int i = 0; i < 12; i++) {
							twelveValues[i] = notesPos[part - 1, subdPerBar * (bar - 1) + subdPerTimeUnit * beat + i];
						}
						addRythmInQuarter(elements, twelveValues);
					}
					block[part, bar] = gcnew Group(elements);
				}
			}

			// Retourner
			return block;

		}

	// Fonctions
	private:
		static void addRythmInQuarter(List<Element^>^ elements_out, array<int, 1>^ twelveValues) {
			
			// Dénombrer les notes présentes
			int nbNotesIn12 = 0; for each (int val in twelveValues) if (val >= 0) nbNotesIn12++;
			
			// Agir en fonction
			if (nbNotesIn12 == 0) {
				// Silence
				elements_out->Add(gcnew Rest(RhythmFigure::_quarter));
			} else if (nbNotesIn12 == 1) {
				// Déterminer la position et la valeur de la note unique (les positions 1, 2, 5, 7, 10, 11 sont impossibles), + tuplet
				int posNote, pitch; for (int i = 0; i < 12; i++) if (twelveValues[i] >= 0) { posNote = i; pitch = twelveValues[i]; }
				Pitch^ pitches = gcnew Pitch(pitch);
				TupletRatio^ uplet_triol = gcnew TupletRatio(RhythmFigure::_eighth, 3, 2);
				// Agir en fonction
				if (posNote == 0) {
					elements_out->Add(gcnew Note(RhythmFigure::_quarter, pitches));
				} else if (posNote == 3) {
					elements_out->Add(gcnew Rest(RhythmFigure::_16th));
					elements_out->Add(gcnew Note(gcnew Rhythm(RhythmFigure::_eighth, 1), pitches));
				} else if (posNote == 4) {
					elements_out->Add(gcnew Tuplet(
						uplet_triol,
						gcnew array<NoteOrRest^> {
							gcnew Rest(RhythmFigure::_eighth),
							gcnew Note(RhythmFigure::_quarter, pitches)
						}
					));
				} else if (posNote == 6) {
					elements_out->Add(gcnew Rest(RhythmFigure::_eighth));
					elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitches));
				} else if (posNote == 8) {
					elements_out->Add(gcnew Tuplet(
						uplet_triol,
						gcnew array<NoteOrRest^> {
							gcnew Rest(RhythmFigure::_quarter),
							gcnew Note(RhythmFigure::_eighth, pitches)
						}
					));
				} else if (posNote == 9) {
					elements_out->Add(gcnew Rest(gcnew Rhythm(RhythmFigure::_eighth, 1)));
					elements_out->Add(gcnew Note(RhythmFigure::_16th, pitches));
				}
			} else if (nbNotesIn12 == 2) {
				// CloudMaker::Pattern, deux pitches, tuplet
				String^ pattern = ""; for each (int val in twelveValues) pattern += (val >= 0 ? "x" : "_");
				Pitch ^pitch1, ^pitch2; {
					for (int i =  0; i <  12; i++) if (twelveValues[i] >= 0) { pitch1 = gcnew Pitch(twelveValues[i]); break; }
					for (int i = 11; i >=  0; i--) if (twelveValues[i] >= 0) { pitch2 = gcnew Pitch(twelveValues[i]); break; }
				}
				TupletRatio^ uplet_triol = TupletRatio::triplet(RhythmFigure::_eighth);
				// Agir selon
				if        (pattern == "x__x________") { elements_out->Add(gcnew Note(             RhythmFigure::_16th                 , pitch1));
				                                        elements_out->Add(gcnew Note(gcnew Rhythm(RhythmFigure::_eighth , 1          ), pitch2));
				} else if (pattern == "x___x_______") {
					elements_out->Add(gcnew Tuplet(
						uplet_triol,
						gcnew array<NoteOrRest^> {
							gcnew Note(RhythmFigure::_eighth, pitch1),
							gcnew Note(RhythmFigure::_quarter, pitch2)
						}
					));
				} else if (pattern == "x_____x_____") { elements_out->Add(gcnew Note(             RhythmFigure::_eighth               , pitch1));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_eighth               , pitch2));
				} else if (pattern == "x_______x___") {
					elements_out->Add(gcnew Tuplet(
						uplet_triol,
						gcnew array<NoteOrRest^> {
							gcnew Note(RhythmFigure::_quarter, pitch1),
							gcnew Note(RhythmFigure::_eighth, pitch2)
						}
					));
				} else if (pattern == "x________x__") { elements_out->Add(gcnew Note(gcnew Rhythm(RhythmFigure::_eighth , 1          ), pitch1));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_16th                 , pitch2));
				} else if (pattern == "___xx_______") { elements_out->Add(gcnew Rest(             RhythmFigure::_16th                         ));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_16th                 , pitch1));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_eighth               , pitch2));
				} else if (pattern == "___x__x_____") { elements_out->Add(gcnew Rest(             RhythmFigure::_16th                         ));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_16th                 , pitch1));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_eighth               , pitch2));
				} else if (pattern == "___x____x___") {
					elements_out->Add(gcnew Tuplet(
						uplet_triol,
						gcnew array<NoteOrRest^> {
							gcnew Rest(RhythmFigure::_eighth),
							gcnew Note(RhythmFigure::_eighth, pitch1),
							gcnew Note(RhythmFigure::_eighth, pitch2)
						}
					));
				} else if (pattern == "___x_____x__") { elements_out->Add(gcnew Rest(             RhythmFigure::_16th                         ));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_eighth               , pitch1));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_16th                 , pitch2));
				} else if (pattern == "____x_x_____") { elements_out->Add(gcnew Rest(             RhythmFigure::_16th                         ));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_16th                 , pitch1));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_eighth               , pitch2));
				} else if (pattern == "____x___x___") {
					elements_out->Add(gcnew Tuplet(
						uplet_triol,
						gcnew array<NoteOrRest^> {
							gcnew Rest(RhythmFigure::_eighth),
							gcnew Note(RhythmFigure::_eighth, pitch1),
							gcnew Note(RhythmFigure::_eighth, pitch2)
						}
					));
				} else if (pattern == "____x____x__") {
					elements_out->Add(gcnew Tuplet(
						uplet_triol,
						gcnew array<NoteOrRest^> {
							gcnew Rest(RhythmFigure::_eighth),
							gcnew Note(RhythmFigure::_eighth, pitch1),
							gcnew Note(RhythmFigure::_eighth, pitch2)
						}
					));
				} else if (pattern == "______x_x___") { elements_out->Add(gcnew Rest(             RhythmFigure::_eighth                       ));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_16th   ,               pitch1));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_16th   ,               pitch2));
				} else if (pattern == "______x__x__") { elements_out->Add(gcnew Rest(             RhythmFigure::_eighth                       ));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_16th   ,               pitch1));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_16th   ,               pitch2));
				} else if (pattern == "________xx__") { elements_out->Add(gcnew Rest(             RhythmFigure::_eighth                       ));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_16th   ,               pitch1));
				                                        elements_out->Add(gcnew Note(             RhythmFigure::_16th   ,               pitch2));
				}
			} else if (nbNotesIn12 == 3) {
				// CloudMaker::Pattern, pitches
				String^ pattern = ""; for each (int val in twelveValues) pattern += (val >= 0 ? "x" : "_");
				array<Pitch^, 1>^ pitches; {
					List<Pitch^>^ pitches_lst = gcnew List<Pitch^>;
					for each (int val in twelveValues) if (val >= 0) pitches_lst->Add(gcnew Pitch(val));
					pitches = pitches_lst->ToArray();
				}
				// Agir selon (reconnaître tous les patterns à 3 notes)
				if (pattern == "x__x__x_____" || pattern == "x__xx_______" || pattern == "x___x_x_____" || pattern == "___xx_x_____" || pattern == "___xx___x___") {
					// Deux doubles, croche
				    elements_out->Add(gcnew Note(RhythmFigure::_16th   , pitches[0]));
				    elements_out->Add(gcnew Note(RhythmFigure::_16th   , pitches[1]));
				    elements_out->Add(gcnew Note(RhythmFigure::_eighth , pitches[2]));
				} else if (pattern == "x__x_____x__") {
					// Syncopette
				    elements_out->Add(gcnew Note(RhythmFigure::_16th   , pitches[0]));
				    elements_out->Add(gcnew Note(RhythmFigure::_eighth , pitches[1]));
				    elements_out->Add(gcnew Note(RhythmFigure::_16th   , pitches[2]));
				} else if (pattern == "x_____x__x__" || pattern == "x_____x_x___" || pattern == "x_______xx__") {
					// Croche, deux doubles
				    elements_out->Add(gcnew Note(RhythmFigure::_eighth , pitches[0]));
				    elements_out->Add(gcnew Note(RhythmFigure::_16th   , pitches[1]));
				    elements_out->Add(gcnew Note(RhythmFigure::_16th   , pitches[2]));
				} else if (pattern == "___x__x__x__" || pattern == "___xx____x__" || pattern == "___x__x_x___" || pattern == "___x____xx__" || pattern == "____x_x_x___" || pattern == "____x_x__x__" || pattern == "____x___xx__" || pattern == "______x_xx__") {
					// Quart de soupir, trois doubles
					elements_out->Add(gcnew Rest(RhythmFigure::_16th               ));
				    elements_out->Add(gcnew Note(RhythmFigure::_16th   , pitches[0]));
				    elements_out->Add(gcnew Note(RhythmFigure::_16th   , pitches[1]));
				    elements_out->Add(gcnew Note(RhythmFigure::_16th   , pitches[2]));
				} else {
					// Triolet
					TupletRatio^ uplet_triol = TupletRatio::triplet(RhythmFigure::_eighth);
					elements_out->Add(gcnew Tuplet(
						uplet_triol,
						gcnew array<NoteOrRest^> {
							gcnew Note(RhythmFigure::_eighth, pitches[0]),
							gcnew Note(RhythmFigure::_eighth, pitches[1]),
							gcnew Note(RhythmFigure::_eighth, pitches[2])
						}
					));
				}
			} else if (nbNotesIn12 >= 4) {
				// Pitches
				array<Pitch^, 1>^ pitches; {
					List<Pitch^>^ pitches_lst = gcnew List<Pitch^>;
					for each (int val in twelveValues) if (val >= 0) pitches_lst->Add(gcnew Pitch(val));
					pitches = pitches_lst->ToArray();
				}
				// Ajout d'un rythme régulier
				if (nbNotesIn12 == 4) {
					for (int i = 0; i < 4; i++) {
						elements_out->Add(gcnew Note(RhythmFigure::_16th, pitches[i]));
					}
				} else {
					TupletRatio^ tuplet = gcnew TupletRatio(RhythmFigure::_16th, nbNotesIn12, 4);
					array<NoteOrRest^>^ elementsInUplet = gcnew array<NoteOrRest^>(nbNotesIn12);
					for (int i = 0; i < nbNotesIn12; i++) elementsInUplet[i] = gcnew Note(RhythmFigure::_16th, pitches[i]);
					elements_out->Add(gcnew Tuplet(tuplet, elementsInUplet));
				}
			}

		}
		static double last(List<ScoreGen::OF_Descente::NNote^>^ list) {
			
			int index = list->Count - 1;

			return index == -1 ? -1 : list[index]->moment;

		}
		static int modCeiling(int n, int mod, int baseVal) {
			
			return modCeiling(n - baseVal, mod) + baseVal;

		}
		static int modCeiling(int n, int mod) {
			
			int rem = n % mod;

			return (rem == 0) ? n : (n - rem + mod);

		}

};
public ref class G_Contraintes : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Contraintes";

		}
		virtual String^ description() override {
			
			return "Usage de contraintes sur une suite de notes régulière à quatre voix.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			// Récupérer les ntoes dans les options
			array<int, 2>^ notes; {
				// Montrer la boîte de dialogue
				ScoreGen::OF_TestContraintes^ dialog = gcnew ScoreGen::OF_TestContraintes(name(), description());
				dialog->ShowDialog();
				// Récupérer les valeurs des options
				array<Object^, 1>^ options = dialog->getOptions();
				if (options == nullptr) return nullptr;
				// Convertir
				notes = (array<int, 2>^)(options[0]);
			}
			int nbNotes = notes->GetUpperBound(1) + 1;

			// Placer les nuances (autres que le 'p' initial)
			int pos_mp, pos_mf, pos_f, pos_ff; {
				pos_mp = (int)Math::Round(((double)(nbNotes - 1)) * 0.40);
				pos_mf = (int)Math::Round(((double)(nbNotes - 1)) * 0.64);
				pos_f  = (int)Math::Round(((double)(nbNotes - 1)) * 0.82);
				pos_ff = (int)Math::Round(((double)(nbNotes - 1)) * 0.94);
			}

			// Créer le bloc, avec quatre parties
			Block^ block = gcnew Block;
			block->addParts(4);

			// Ajouter mesures
			int notesPerBar = 16;
			int nbBars = (nbNotes + notesPerBar - 1) / notesPerBar;
			for (int bar = 1; bar <= nbBars; bar++) {
				block->addBar("C");
				for (int part = 1; part <= 4; part++) {
					List<Element^>^ elements = gcnew List<Element^>;
					for (int ii = (bar - 1) * notesPerBar; ii < bar * notesPerBar; ii++) {
						if (ii < nbNotes) {
							Pitch^ pitch = gcnew Pitch(notes[part - 1, ii]);
							array<Words::Words^, 1>^ texts;
							if      (ii == 0     ) texts = gcnew array<Words::Words^, 1> { qWords("!p"), qWords(".pizz.") };
							else if (ii == pos_mp) texts = gcnew array<Words::Words^, 1> { qWords("!mp") };
							else if (ii == pos_mf) texts = gcnew array<Words::Words^, 1> { qWords("!mf") };
							else if (ii == pos_f ) texts = gcnew array<Words::Words^, 1> { qWords("!f" ) };
							else if (ii == pos_ff) texts = gcnew array<Words::Words^, 1> { qWords("!ff") };
							else                   texts = nullptr;
							elements->Add(gcnew Note(RhythmFigure::_16th, pitch, texts));
						} else {
							elements->Add(gcnew Rest(RhythmFigure::_16th));
						}
					}
					block[part, bar] = gcnew Group(elements);
				}
			}

			// Retourner
			return gcnew Score(block, gcnew array<Clef^, 1> { Clef::G2, Clef::G2, Clef::C3, Clef::F4 });

		}

};
public ref class G_ThueInRythm : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Thue-Morse en rythme";

		}
		virtual String^ description() override {
			
			return "Rythme basé sur la suite de Thue-Morse, éventuellement réparti entre plusieurs instruments.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			// Récupérer les options
			array<bool, 2>^ whenNotes; {
				// Montrer la boîte de dialogue
				ScoreGen::OF_ThueInRythm^ dialog = gcnew ScoreGen::OF_ThueInRythm(name(), description());
				dialog->ShowDialog();
				// Récupérer les valeurs des options
				array<Object^, 1>^ options = dialog->getOptions();
				if (options == nullptr) return nullptr;
				// Convertir
				whenNotes = (array<bool, 2>^)(options[0]);
			}

			// Calculs préalables
			int nbInstruments = 1 + whenNotes->GetUpperBound(0);
			int nbNotes       = 1 + whenNotes->GetUpperBound(1);
			int notesPerBar = 16;
			int nbBars = (nbNotes + notesPerBar - 1) / notesPerBar;

			// Créer le block
			Block^ block = gcnew Block(nbInstruments);
			block->addBars(nbBars, "C");

			// Ajouter mesures
			for (int part = 1; part <= nbInstruments; part++) {
				for (int bar = 1; bar <= nbBars; bar++) {
					List<Element^>^ elements = gcnew List<Element^>;
					for (int ii = (bar - 1) * notesPerBar; ii < bar * notesPerBar; ii += 4) {
						addRythmInQuarter(elements, gcnew array<bool, 1> {
							whenNotes[part - 1, ii    ],
							whenNotes[part - 1, ii + 1],
							whenNotes[part - 1, ii + 2],
							whenNotes[part - 1, ii + 3]
						});
					}
					block[part, bar] = gcnew Group(elements);
				}
			}

			// Retourner
			return block;

		}

	// Fonctions
	private:
		static void addRythmInQuarter(List<Element^>^ elements_out, array<bool, 1>^ fourValues) {
			
			String^ pattern = (fourValues[0] ? "x" : "_") + "" + (fourValues[1] ? "x" : "_") + "" + (fourValues[2] ? "x" : "_") + "" + (fourValues[3] ? "x" : "_"); 
			Pitch^ pitch = gcnew Pitch(60);

			if (pattern == "____") {
				// Silence
				elements_out->Add(gcnew Rest(RhythmFigure::_quarter));
			} else if (pattern == "x___") {
				// Noire
				elements_out->Add(gcnew Note(RhythmFigure::_quarter, pitch));
			} else if (pattern == "_x__") {
				// Quart de soupir, croche pointée
				elements_out->Add(gcnew Rest(RhythmFigure::_16th));
				elements_out->Add(gcnew Note(gcnew Rhythm(RhythmFigure::_eighth, 1), pitch));
			} else if (pattern == "__x_") {
				// Demi soupir, croche
				elements_out->Add(gcnew Rest(RhythmFigure::_eighth));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
			} else if (pattern == "___x") {
				// Demi soupir pointé, double croche
				elements_out->Add(gcnew Rest(gcnew Rhythm(RhythmFigure::_eighth, 1)));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			} else if (pattern == "xx__") {
				// Double croche, croche pointée
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(gcnew Rhythm(RhythmFigure::_eighth, 1), pitch));
			} else if (pattern == "x_x_") {
				// Deux croches
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
			} else if (pattern == "x__x") {
				// Croche pointée, double croche
				elements_out->Add(gcnew Note(gcnew Rhythm(RhythmFigure::_eighth, 1), pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			} else if (pattern == "_xx_") {
				// Quart de soupir, double croche, croche
				elements_out->Add(gcnew Rest(RhythmFigure::_16th));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
			} else if (pattern == "_x_x") {
				// Quart de soupir, croche, double croche
				elements_out->Add(gcnew Rest(RhythmFigure::_16th));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			} else if (pattern == "__xx") {
				// Demi soupir, deux doubles croches
				elements_out->Add(gcnew Rest(RhythmFigure::_eighth));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			} else if (pattern == "xxx_") {
				// Deux doubles croches, croche
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
			} else if (pattern == "xx_x") {
				// Double croche, croche, double croche
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			} else if (pattern == "x_xx") {
				// Croche, deux doubles croches
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			} else if (pattern == "_xxx") {
				// Quart de soupir, trois doubles croches
				elements_out->Add(gcnew Rest(RhythmFigure::_16th));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			} else if (pattern == "xxxx") {
				// Quatre doubles croches
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			}

		}

};
public ref class G_SquareFreeSequenceInRhythm : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Suite sans triples en rythme";

		}
		virtual String^ description() override {
			
			return "Rythme basé sur une suite sans triples à 2, 3 ou 4 éléments.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			// Récupérer les options
			int nbNotes;
			array<int, 1>^ durations_16th;
			{
				// Montrer la boîte de dialogue
				ScoreGen::OF_SquareFreeSequenceInRhythm^ dialog = gcnew ScoreGen::OF_SquareFreeSequenceInRhythm(name(), description());
				dialog->ShowDialog();
				// Récupérer les valeurs des options
				array<Object^, 1>^ options = dialog->getOptions();
				if (options == nullptr) return nullptr;
				// Convertir
				nbNotes        = (int           )(options[0]);
				durations_16th = (array<int, 1>^)(options[1]);
			}
			int nbDifferentDurations = durations_16th->Length;;

			// Créer la suite des durées
			array<int, 1>^ durationsSeq; {
				// Récupérer la suite mathématique
				if (nbDifferentDurations == 2) {
					array<bool, 1>^ thue = Seq::thueMorse(nbNotes);
					durationsSeq = gcnew array<int, 1>(nbNotes);
					for (int i = 0; i < nbNotes; i++) durationsSeq[i] = thue[i] ? 1 : 0;
				} else if (nbDifferentDurations == 3) {
					durationsSeq = Seq::squareFreeWord3(nbNotes);
				} else if (nbDifferentDurations == 4) {
					durationsSeq = Seq::squareFreeWord4(nbNotes);
				} else {
					throw gcnew Exception;
				}
				// Mettre les bonnes durées
				for (int i = 0; i < nbNotes; i++) durationsSeq[i] = durations_16th[durationsSeq[i]];
			}

			// Créer le tableau d'occupation du temps
			int totalDuration_16th; {
				// Faire la somme de toutes les durées
				totalDuration_16th = 0;
				for each (int dur in durationsSeq) totalDuration_16th += dur;
				// Remplacer la dernière durée par une double croche simple
				totalDuration_16th = totalDuration_16th - durationsSeq[durationsSeq->Length - 1] + 1;
				// Compléter la dernière mesure
				totalDuration_16th = ((totalDuration_16th + 15) / 16) * 16;
			}
			int nbBars = totalDuration_16th / 16;
			array<bool, 1>^ played; {
				played = gcnew array<bool, 1>(totalDuration_16th);
				int cpt = 0;
				for each (int dur in durationsSeq) {
					played[cpt] = true;
					cpt += dur;

				}
			}

			// Récupérer les éléments
			array<List<Element^>^, 1>^ elements = gcnew array<List<Element^>^, 1>(nbBars); {
				for (int bar = 0; bar < nbBars; bar++) {
					elements[bar] = gcnew List<Element^>;
					for (int i = 16 * bar; i < 16 * (bar + 1); i += 4) {
						addRythmInQuarter(elements[bar], gcnew array<bool, 1>{ played[i], played[i + 1], played[i + 2], played[i + 3] });
					}
				}
			}

			// Créer le block
			Block^ block = gcnew Block(1);
			block->addBars(nbBars, "C");

			// Remplir
			for (int bar = 0; bar < nbBars; bar++) {
				block[1, bar + 1] = gcnew Group(elements[bar]);
			}

			// Retourner
			return block;

		}

	// Fonctions
	private:
		static void addRythmInQuarter(List<Element^>^ elements_out, array<bool, 1>^ fourValues) {
			
			String^ pattern = (fourValues[0] ? "x" : "_") + "" + (fourValues[1] ? "x" : "_") + "" + (fourValues[2] ? "x" : "_") + "" + (fourValues[3] ? "x" : "_"); 
			Pitch^ pitch = gcnew Pitch(60);

			if (pattern == "____") {
				// Silence
				elements_out->Add(gcnew Rest(RhythmFigure::_quarter));
			} else if (pattern == "x___") {
				// Noire
				elements_out->Add(gcnew Note(RhythmFigure::_quarter, pitch));
			} else if (pattern == "_x__") {
				// Quart de soupir, croche pointée
				elements_out->Add(gcnew Rest(RhythmFigure::_16th));
				elements_out->Add(gcnew Note(gcnew Rhythm(RhythmFigure::_eighth, 1), pitch));
			} else if (pattern == "__x_") {
				// Demi soupir, croche
				elements_out->Add(gcnew Rest(RhythmFigure::_eighth));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
			} else if (pattern == "___x") {
				// Demi soupir pointé, double croche
				elements_out->Add(gcnew Rest(gcnew Rhythm(RhythmFigure::_eighth, 1)));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			} else if (pattern == "xx__") {
				// Double croche, croche pointée
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(gcnew Rhythm(RhythmFigure::_eighth, 1), pitch));
			} else if (pattern == "x_x_") {
				// Deux croches
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
			} else if (pattern == "x__x") {
				// Croche pointée, double croche
				elements_out->Add(gcnew Note(gcnew Rhythm(RhythmFigure::_eighth, 1), pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			} else if (pattern == "_xx_") {
				// Quart de soupir, double croche, croche
				elements_out->Add(gcnew Rest(RhythmFigure::_16th));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
			} else if (pattern == "_x_x") {
				// Quart de soupir, croche, double croche
				elements_out->Add(gcnew Rest(RhythmFigure::_16th));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			} else if (pattern == "__xx") {
				// Demi soupir, deux doubles croches
				elements_out->Add(gcnew Rest(RhythmFigure::_eighth));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			} else if (pattern == "xxx_") {
				// Deux doubles croches, croche
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
			} else if (pattern == "xx_x") {
				// Double croche, croche, double croche
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			} else if (pattern == "x_xx") {
				// Croche, deux doubles croches
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			} else if (pattern == "_xxx") {
				// Quart de soupir, trois doubles croches
				elements_out->Add(gcnew Rest(RhythmFigure::_16th));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			} else if (pattern == "xxxx") {
				// Quatre doubles croches
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitch));
			}

		}

};
