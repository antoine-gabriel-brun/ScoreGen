#pragma region head

	#pragma once

	#include "ScoreGenLib.h"
	using namespace ScoreGenLib;
	using namespace ScoreGenLib::Generators;
	using namespace CloudMaker;

#pragma endregion

// Dalle : Notes aléatoires après Prologue
public ref class G_Dalle_Prologh : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Dalle : Notes aléatoires après Prologue";

		}
		virtual String^ description() override {
			
			return "Préférence pour les mouvements ascendants, absence de notes répétées, modulation d'ambitus, silences après certains sommets (possibilité de moduler le ratio).";

		}
		virtual ScoreAndInfos^ generate() override {
			
			// Générer la suite de notes
			int nbPitchesInSeq = 280;
			array<int, 1>^ seq = genPitchesSeq(nbPitchesInSeq);

			// Ajouter des silences (-1) après certains sommets
			array<int, 1>^ pitchesAndRests = genPitchesAndRests(seq, nbPitchesInSeq);
			int nbElements = pitchesAndRests->Length;

			// Créer le bloc
			Block^ block = gcnew Block(1);
			block->addBar(gcnew Signature(nbElements, 32));

			// Créer les éléments
			array<Element^, 1>^ elements = gcnew array<Element^, 1>(nbElements);
			for (int i = 0; i < nbElements; i++) {
				if (pitchesAndRests[i] == -1) {
					elements[i] = gcnew Rest(RhythmFigure::_32nd);
				} else {
					elements[i] = gcnew Note(RhythmFigure::_32nd, gcnew Pitch(pitchesAndRests[i]));
				}
			}

			// Ajouter la mesure unique
			block[1, 1] = gcnew Group(elements);

			// Retourner
			return block;

		}

	// Fonctions
	private:
		array<int, 1>^ genPitchesSeq(int nb) {
		
			array<int, 1>^ ans = gcnew array<int, 1>(nb);
			ans[0] = 48; ans[1] = 54; ans[2] = 58;

			array<int, 1>^ minMaxVar = gcnew array<int, 1>(3);
			for (int i = 3; i < nb; i++) {
				getMinMaxVar(minMaxVar, ((double)(i - 3)) / ((double)(nb - 4)));
				int next = ans[i - 1] + Rand::integer(1, 2 * minMaxVar[2]);
				next = (next - minMaxVar[0]) % (minMaxVar[1] - minMaxVar[0] + 1) + minMaxVar[0];
				ans[i] = next;
			}

			return ans;

		}
		void getMinMaxVar(array<int, 1>^ minMaxVar_out, double t) {
			
			double min = 48 - 12 * t;
			double max = 66 + 12 * t;
			double var = 5 + 4 * t;

			minMaxVar_out[0] = (int)Math::Round(min);
			minMaxVar_out[1] = (int)Math::Round(max);
			minMaxVar_out[2] = (int)Math::Round(var);

		}
		array<int, 1>^ genPitchesAndRests(array<int, 1>^ seq, int nbPitchesInSeq) {
		
			List<int>^ out = gcnew List<int>;

			for (int i = 0; i < nbPitchesInSeq; i++) {
				if (i > 2 && i < nbPitchesInSeq - 3) {
					if (seq[i - 1] > seq[i - 2] && seq[i - 1] > seq[i]) {
						int pause = 0;
						if (Rand::double01() < 0.2) {
							if (Rand::double01() < 0.2) pause = Rand::integer(12, 36);
							else                        pause = Rand::integer(1, 12);
						}
						for (int i = 0; i < pause; i++) out->Add(-1);
					}
				}
				out->Add(seq[i]);
			}

			return out->ToArray();
		
		}

};

// Dalle : Accords aléatoires sur la Dalle
public ref class G_Dalle_DalChords : public Generator {
	
	// Options
	private:
		static const int nbChordsInSeq = 190;
		static const String^ mode = "ravel";
			// major - minor - penta - ravel - arabic - hexa - maessian 1 - maessian 2 - harmonic - chroma 6 - chroma 8 - chroma 10 - chroma 12

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Dalle : Accords aléatoires sur la Dalle (" + mode + ")";

		}
		virtual String^ description() override {
			
			return "Accords de durées très variables, avec pauses, composés de 5 à 12 sons issus d'un même mode transposé.";

		}
		virtual ScoreAndInfos^ generate() override {

			// Générer la suite d'accords
			array<array<int, 1>^, 1>^ seq = genChordsSeq(nbChordsInSeq);
			
			// Générer les durées
			array<int, 1>^ durations = genDurations(nbChordsInSeq);

			// Calculer la durée générale (en doubles croches)
			int dur; {
				dur = 0;
				for each (int d in durations) dur += d;
			}

			// Ajuster la durée générale sur le dernier accord pour obtenir un multiple de 16
			{
				if (dur % 16 != 0) {
					int add = 16 - (dur % 16);
					dur += add;
					durations[nbChordsInSeq - 1] += add;
				}
			}
			int nbBars = dur / 16;

			// Créer le bloc
			Block^ block = gcnew Block(1);
			block->addBars(nbBars, qSignature("C"));

			// Créer les éléments pour chaque voix
			array<Element^, 1>^ elements = gcnew array<Element^, 1>(dur);
			int cpt = 0;
			for (int i = 0; i < nbChordsInSeq; i++) {
				array<int, 1>^ chordInSeq = seq[i];
				int thisChordDur = durations[i];
				int nbPitches = chordInSeq->Length;
				array<Pitch^, 1>^ pitches = gcnew array<Pitch^, 1>(nbPitches);
				for (int j = 0; j < nbPitches; j++) pitches[j] = gcnew Pitch(chordInSeq[j]);
				for (int j = 0; j < thisChordDur; j++) {
					NoteDecorations^ deco = gcnew NoteDecorations(j < thisChordDur - 1, false, false);
					elements[cpt++] = gcnew Note(RhythmFigure::_16th, pitches, deco);
				}
			}

			// Ajouter les éléments pour chaque mesure
			for (int bar = 0; bar < nbBars; bar++) {
				array<Element^, 1>^ elementsHere = gcnew array<Element^, 1>(16);
				for (int i = 0; i < 16; i++) {
					elementsHere[i] = elements[16 * bar + i];
				}
				block[1, bar + 1] = gcnew Group(elementsHere);
			}

			// Retourner
			return block;

		}

	// Fonctions
	private:
		array<array<int, 1>^, 1>^ genChordsSeq(int nb) {
			
			// Créer le mode de base
			array<int, 1>^ notesInMode;
			int modeLen;
			{
				if      (mode == "major"     ) notesInMode = gcnew array<int, 1>{ 0, 2, 4, 5, 7, 9, 11 };
				else if (mode == "minor"     ) notesInMode = gcnew array<int, 1>{ 0, 2, 3, 5, 7, 8, 11 };
				else if (mode == "penta"     ) notesInMode = gcnew array<int, 1>{ 0, 2, 5, 7, 9 };
				else if (mode == "ravel"     ) notesInMode = gcnew array<int, 1>{ 0, 2, 4, 6, 8, 10 };
				else if (mode == "arabic"    ) notesInMode = gcnew array<int, 1>{ 0, 2, 3, 6, 7, 8, 11 };
				else if (mode == "hexa"      ) notesInMode = gcnew array<int, 1>{ 0, 3, 4, 7, 8, 11 };
				else if (mode == "maessian 1") notesInMode = gcnew array<int, 1>{ 0, 1, 3, 4, 6, 7, 9, 10 };
				else if (mode == "maessian 2") notesInMode = gcnew array<int, 1>{ 0, 1, 2, 4, 5, 6, 8, 9, 10 };
				else if (mode == "harmonic"  ) notesInMode = gcnew array<int, 1>{ 0, 2, 4, 6, 7, 9, 10 };
				else if (mode == "chroma 6"  ) notesInMode = gcnew array<int, 1>{ 0, 1, 2, 3, 4, 5 };
				else if (mode == "chroma 8"  ) notesInMode = gcnew array<int, 1>{ 0, 1, 2, 3, 4, 5, 6, 7 };
				else if (mode == "chroma 10" ) notesInMode = gcnew array<int, 1>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
				else if (mode == "chroma 12" ) notesInMode = gcnew array<int, 1>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
				else throw gcnew Exception;
				modeLen = notesInMode->Length;
			}

			// Créer le tableau
			array<array<int, 1>^, 1>^ ans = gcnew array<array<int, 1>^, 1>(nb);
			array<int, 1>^ minMax = gcnew array<int, 1>(2);
			int min, max;
			for (int i = 0; i < nb; i++) {
				// Transposer aléatoirement le mode
				array<int, 1>^ modeTransp = gcnew array<int, 1>(modeLen); {
					int add = Rand::integer(0, 12);
					for (int j = 0; j < modeLen; j++) modeTransp[j] = (notesInMode[j] + add) % 12;
				}
				// Déterminer l'ambitus (et s'assurer que l'on dispose d'au moins une octave)
				getPitchMinMax(minMax, ((double)i) / ((double)(nb - 1)));
				min = minMax[0]; max = minMax[1]; max = Math::Max(max, min + 11);
				// Répartir dans l'ambitus possible
				for (int j = 0; j < modeLen; j++) {
					int thisNoteMin, thisNoteMax; {
						thisNoteMin = modeTransp[j];
						while (thisNoteMin < min) thisNoteMin += 12;
						thisNoteMax = thisNoteMin;
						while (thisNoteMax + 12 <= max) thisNoteMax += 12;
					}
					int nbOptions = (thisNoteMax - thisNoteMin) / 12 + 1;
					modeTransp[j] = thisNoteMin + 12 * Rand::integer(0, nbOptions);
				}
				Array::Sort(modeTransp);
				// Conserver le résultat
				ans[i] = modeTransp;
			}

			// Retourner
			return ans;

		}
		void getPitchMinMax(array<int, 1>^ minMax_out, double t) {
			
			double min = 48 - 12 * t;
			double max = 60 + 20 * t;

			minMax_out[0] = (int)Math::Round(min);
			minMax_out[1] = (int)Math::Round(max);

		}
		array<int, 1>^ genDurations(int nb) {
		
			// Générer des durées aléatoires
			array<int, 1>^ ans = gcnew array<int, 1>(nb); {
				array<int, 1>^ minMax = gcnew array<int, 1>(2);
				int min, max;
				for (int i = 0; i < nb; i++) {
					getDurMinMax(minMax, ((double)i) / ((double)(nb - 1)));
					min = minMax[0];
					max = minMax[1];
					double val01 = Rand::double01();
					val01 *= val01;
					ans[i] = (int)Math::Floor(min + (max + 0.99999 - min) * val01);
				}
			}

			// Réorganiser
			double reorgRatio = 8.5; {
				int nbReorg = (int)Math::Round(reorgRatio * (nb - 3));
				for (int n = 0; n < nbReorg; n++) {
					int i = Rand::integer(0, nb - 3);
					int a = ans[i];
					int b = ans[i + 1];
					int c = ans[i + 2];
					int d = ans[i + 3];
					if (a < b && b > c && c < d) {
						ans[i + 1] = c;
						ans[i + 2] = b;
					}
				}
			}
			
			// Retourner
			return ans;

		}
		void getDurMinMax(array<int, 1>^ minMax_out, double t) {
			
			int min, max;

			if      (t < 0.3333) min = 1;
			else if (t < 0.6667) min = 2;
			else                 min = 1;

			if      (t < 0.25) max = 18;
			else if (t < 0.5 ) max = 16;
			else if (t < 0.75) max = 28;
			else               max = 13;

			minMax_out[0] = min;
			minMax_out[1] = max;

		}

};
