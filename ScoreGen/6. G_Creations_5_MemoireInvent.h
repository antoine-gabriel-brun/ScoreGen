#pragma region head

	#pragma once

	#include "ScoreGenLib.h"
	using namespace ScoreGenLib;
	using namespace ScoreGenLib::Generators;
	using namespace CloudMaker;

	using namespace System;
	using namespace System::Collections;
	using namespace System::Collections::Generic;

#pragma endregion

// Markov
public ref class G_Memoire_BeforeExamples_Markov : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "TEST mémoire";

		}
		virtual String^ description() override {
			
			return "TEST mémoire";

		}
		virtual ScoreAndInfos^ generate() override {

			// Créer la chaîne de Markov
			array<double, 2>^ probas_dbl = gcnew array<double, 2>{
				{ 0.6, 0.4, 0, 0 },
				{ 0.5, 0, 0.5, 0 },
				{ 0, 0.33, 0.33, 0.33 },
				{ 1, 0, 0, 0 }
			};
			array<TimeVariable^, 2>^ probas = gcnew array<TimeVariable^, 2>(4, 4); {
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						probas[i, j] = gcnew TimeVariable_Const(
							probas_dbl[i, j]
						);
					}
				}
			}
			MathForMusic::Markov^ chain = gcnew Markov(4, probas, false);

			// Créer les éléments
			int nb = 40;
			array<int, 1>^ res;
			{
				res = gcnew array<int, 1>(nb);
				res[0] = 0;
				chain->initialize(res[0]);
				for (int i = 1; i < nb; i++) {
					res[i] = chain->next(0);
				}
			}

			// Créer le bloc
			Block^ block = gcnew Block(2);
			block->addBars(nb, qSignature("C"));

			// Ajouter tous les accords
			for (int bar = 1; bar <=nb; bar++) {
				int chordIndex = res[bar - 1];
				if      (chordIndex == 0) { block[1, bar] = qNote("w|60|64|67"); block[2, bar] = qNote("w|48"); }
				else if (chordIndex == 1) { block[1, bar] = qNote("w|60|65|69"); block[2, bar] = qNote("w|41"); }
				else if (chordIndex == 2) { block[1, bar] = qNote("w|60|65|69"); block[2, bar] = qNote("w|38"); }
				else if (chordIndex == 3) { block[1, bar] = qNote("w|59|62|67"); block[2, bar] = qNote("w|43"); }
			}

			// Retourner
			return block;

		}

};

// Exemples
public ref class G_StructureDesExemples : public Generator {
	public:
		virtual String^ name() override {
			return "Structure minimale d'un objet de type Generator";
		}
		virtual String^ description() override {
			return "Ce générateur crée un duo avec 10 mesures vides en 9/8.";
		}
		virtual ScoreAndInfos^ generate() override {
			Block^ block = gcnew Block(2);
			block->addBars(10, qSignature("9/8"));
			return block;
		}
};
public ref class G_Bartok : public Generator {
	public:
		virtual String^ name() override { return ""; }
		virtual String^ description() override { return ""; }
		virtual ScoreAndInfos^ generate() override {
			Block^ block = gcnew Block(4);
			block->addBars(2, qSignature("9/8"));
			block->addBars(1, qSignature("6/8"));
			block[1, 2] = qGroup("e. 16th(|E4<!p> 16th|A4 16th)|D5 q._(|C#5 q_|C#5 16th|C#5 16th)|G#4");
			block[1, 3] = qGroup("q.|G4 q._|Bb4");
			block[2, 1] = qGroup("q. e|Eb4<!p><:sempre tenuto> e|Eb4 e_|Eb4 e|Eb4 e|Eb4 e_|Eb4");
			block[2, 2] = qGroup("e|Eb4 e|E4 e_|E4 e|E4 e|G4 e_|E4 e|E4 e|D#4 e(|E4");
			block[2, 3] = qGroup("e)|F4 e|F4 e_|F4 e|F4 e|C#4 e_|D4");
			block[3, 1] = qGroup("q. e|D4<!p><:sempre tenuto> e|D4 e_|D4 e|D4 e|D4 e_|D4");
			block[3, 2] = qGroup("e|D4 e|D4 e_|D4 e|D4 e|D4 e_|D4 e|D4 e|D4 e(|D4");
			block[3, 3] = qGroup("e)|C4 e|B3 e_|C4 e|C4 e|C4 e_|C4");
			block[4, 1] = qGroup("h._(|Bb2<!p> q_|Bb2 16th|Bb2 16th)|Bb3");
			block[4, 2] = qGroup("h._|Bb3 q._|Bb3");
			block[4, 3] = qGroup("q.|Bb3 q._|G3");
			return block;
		}
};
public ref class G_ChromaFreq : public Generator {
	public:
		virtual String^ name() override { return ""; }
		virtual String^ description() override { return ""; }
		virtual ScoreAndInfos^ generate() override {
			int nb = 13;
			int basePitch = 60;
			array<Note^, 1>^ notes = gcnew array<Note^, 1>(nb);
			for (int i = 0; i < nb; i++) {
				int midi = basePitch + i;
				notes[i] = gcnew Note(
					RhythmFigure::_whole,
					gcnew Pitch(midi),
					qWords("." + midiToFreq(midi))
				);
			}
			Group^ group = gcnew Group(notes);
			return gcnew Block(group);
		}
	private:
		static String^ midiToFreq(double midi) {
		
			double f = 440 * Math::Pow(2, ((midi - 69) / 12));

			return "" + Math::Round(f, 0);
		
		}
};
public ref class G_Automations : public Generator {
	public:
		virtual String^ name() override { return ""; }
		virtual String^ description() override { return ""; }
		virtual ScoreAndInfos^ generate() override {
			int nb = 192;
			array<NoteOrRest^, 1>^ elements = gcnew array<NoteOrRest^, 1>(nb);
			TimeVariable^ density = gcnew TimeVariable_Lin(0.1, 1);
			TimeVariable^ low     = gcnew TimeVariable_Lin(60, 48);
			TimeVariable^ high    = gcnew TimeVariable_Lin(60, 72);
			for (int i = 0; i < nb; i++) {
				double densityNow =    density->at(i, nb);
				int        lowNow = round( low->at(i, nb));
				int       highNow = round(high->at(i, nb));
				if (Rand::double01() < densityNow) {
					int midi = Rand::integer(lowNow, highNow + 1);
					elements[i] = qNote("16th|" + midi);
				} else {
					elements[i] = qRest("16th");
				}
			}
			Group^ group = gcnew Group(elements);
			return gcnew Block(group);
		}
	private:
		static int round(double x) {
			
			return (int)Math::Round(x);

		}
};
public ref class G_Melody : public Generator {
	private:
		static const int nbSuccessiveModes = 48;
		static const bool randomize = false;
	public:
		virtual String^ name() override { return ""; }
		virtual String^ description() override { return ""; }
		virtual ScoreAndInfos^ generate() override {
			if (randomize) Rand::randomize();
			// Enumérer les 12 modes pentatoniques possibles
			array<array<int, 1>^, 1>^ possibleModes = getAllPentaModes();
			// Créer une série de modes pentatoniques dans le temps
			array<array<int, 1>^, 1>^ modesSeq = createModesSeq(possibleModes);
			// Définir une durée en croches pour chaque mode de la séquence
			array<int, 1>^ durations = createDurations();
			int totalDurationEighths = 0; for each (int d in durations) totalDurationEighths += d;
			// Ajouter le temps nécessaire pour éviter une dernière mesure incomplète
			if (totalDurationEighths % 8 != 0) {
				int add = 8 - (totalDurationEighths % 8);
				totalDurationEighths += add;
				durations[nbSuccessiveModes - 1] += add;
			}
			// Réaliser l'accompagnement de piano à deux portées, synchronisé fortement sur les durées et faiblement sur les mesures
			array<List<Element^>^, 1>^ pianoVoices = createPianoVoices(modesSeq, durations);
			// Réaliser la voix de violon à une portée, synchronisée fortement sur les mesures
			List<Element^>^ violinVoice = createViolinVoice(modesSeq, durations, totalDurationEighths);
			// Couper sur les mesures
			array<List<Element^>^, 2>^ elements = getElementsAsGrid(pianoVoices, violinVoice, totalDurationEighths);
			// Créer et retourner le bloc
			int nbParts = 3;
			int nbBars = elements->GetUpperBound(1) + 1;
			Block^ block = gcnew Block(nbParts);
			block->addBars(nbBars, qSignature("C"));
			for (int part = 0; part < nbParts; part++) {
				for (int bar = 0; bar < nbBars; bar++) {
					block[part + 1, bar + 1] = gcnew Group(elements[part, bar]);
				}
			}
			return block;
		}
	private:
		static array<array<int, 1>^, 1>^ getAllPentaModes() {
			array<array<int, 1>^, 1>^ ans = gcnew array<array<int, 1>^, 1>(12);
			ans[0] = gcnew array<int, 1>{ 0, 2, 4, 7, 9 };
			for (int i = 1; i < 12; i++) {
				ans[i] = copy(ans[i - 1]);
				for (int j = 0; j < 5; j++) ans[i][j] = (ans[i][j] + 1) % 12;
				Array::Sort(ans[i]);
			}
			scramble(ans);
			return ans;
		}
		static array<array<int, 1>^, 1>^ createModesSeq(array<array<int, 1>^, 1>^ possibleModes) {
			array<int, 1>^ seq = Seq::fractal010210(nbSuccessiveModes);
			array<array<int, 1>^, 1>^ ans = gcnew array<array<int, 1>^, 1>(nbSuccessiveModes);
			for (int i = 0; i < nbSuccessiveModes; i++) {
				ans[i] = possibleModes[seq[i] % 12];
			}
			return ans;
		}
		static array<int, 1>^ createDurations() {
			array<int, 1>^ ans = gcnew array<int, 1>(nbSuccessiveModes);
			for (int i = 0; i < nbSuccessiveModes; i++) {
				ans[i] = Rand::integer(1, 10);
			}
			return ans;
		}
		static array<List<Element^>^, 2>^ getElementsAsGrid(array<List<Element^>^, 1>^ pianoVoices, List<Element^>^ violinVoice, int totalDurationEighths) {
			array<List<Element^>^, 1>^ elements = gcnew array<List<Element^>^, 1>{violinVoice, pianoVoices[0], pianoVoices[1]};
			int nbBars = totalDurationEighths / 8;
			// Créer la grille de sortie
			array<List<Element^>^, 2>^ grid = gcnew array<List<Element^>^, 2>(3, nbBars);
			for (int i = 0; i < 3; i++) for (int j = 0; j < nbBars; j++) grid[i, j] = gcnew List<Element^>;
			// Couper
			for (int i = 0; i < 3; i++) {
				int cptBar = 0;
				int barLen_1024th = 0;
				for each (Element^ e in elements[i]) {
					grid[i, cptBar]->Add(e);
					barLen_1024th += e->duration_1024th;
					if (barLen_1024th > 1024) {
						throw gcnew Exception("Un élément passe par-dessus une barre de mesure.");
					} else if (barLen_1024th == 1024) {
						cptBar++;
						barLen_1024th = 0;
					}
				}
			}
			// Retourner
			return grid;
		}
	private:
		// Piano
		static array<List<Element^>^, 1>^ createPianoVoices(array<array<int, 1>^, 1>^ modesSeq, array<int, 1>^ durations) {
			array<List<Element^>^, 1>^ ans = gcnew array<List<Element^>^, 1>(2);
			for (int i = 0; i < 2; i++) ans[i] = gcnew List<Element^>;
			int pos = 0;
			for (int i = 0; i < nbSuccessiveModes; i++) {
				appendPianoChord(ans, ((double)i) / (nbSuccessiveModes - 1), modesSeq[i], pos, durations[i]);
				pos += durations[i];
			}
			return ans;
		}
		static void appendPianoChord(array<List<Element^>^, 1>^ out, double progress, array<int, 1>^ mode, int pos, int nbEighths) {
			// Créer deux accords réalisant le mode (premier plus grave que second, le second ne sera pas toujours utilisé)
			array<int, 1> ^chord1, ^chord2; {
				bool bad;
				do {
					chord1 = createChord(mode, progress);
					chord2 = createChord(mode, progress);
					bad = false;
					// Basses différentes ? et réordonner (basse 1 < basse 2)
					if (chord1[0] == chord2[0]) {
						bad = true; continue;
					} else if (chord1[0] > chord2[0]) {
						auto e = chord1;
						chord1 = chord2;
						chord2 = e;
					}
					// Hauts bien ordonnés ?
					if (chord1[4] >= chord2[4]) bad = true;
				} while (bad);
			}
			// L'accord à ajouter (a) part sur la mesure, (b) contient un premier temps de mesure, ou (c) est inclus dans une mesure
			char type;
			int before, after;
			{
				if (pos % 8 == 0) {
					type = 'a';
				} else {
					int posNextBar = (pos / 8 + 1) * 8;
					if (pos + nbEighths > posNextBar) {
						type = 'b';
						before = posNextBar - pos;
						after = nbEighths - before;
					} else {
						type = 'c';
					}
				}
			}
			// Ajouter des éléments aux deux voix en fonction de ces paramètres
			if      (type == 'a') appendPianoChord_onBarLine        (out, nbEighths, progress, chord1, chord2);
			else if (type == 'b') appendPianoChord_straddlingBarLine(out, nbEighths, progress, chord1, chord2, before, after);
			else if (type == 'c') appendPianoChord_entirelyInBar    (out, nbEighths, progress, chord1, chord2);
		}
		static TimeVariable^ var_lo = gcnew TimeVariable_Lin(43, 38);
		static TimeVariable^ var_hi = gcnew TimeVariable_Lin(69, 80);
		static array<int, 1>^ createChord(array<int, 1>^ mode, double progress) {
			// Décider de l'ambitus total
			int lo = (int)Math::Round(var_lo->at(progress));
			int hi = (int)Math::Round(var_hi->at(progress));
			// Restreindre parfois l'ambitus (2 fois sur 7 dans la seconde moitié de la pièce)
			if (progress > 0.5 && Rand::double01() < 2. / 7.) lo += (2 * (hi - lo)) / 5;
			// Distribuer chaque note du mode n'importe où dans l'ambitus (en essayant d'éviter les secondes dans le grave)
			array<int, 1>^ chord = gcnew array<int, 1>(5); {
				int nbPbs = 0;
				bool pb;
				do {
					for (int i = 0; i < 5; i++) {
						int baseNote = mode[i];
						int min = mode[i], max = mode[i];
						while (min < lo) min += 12;
						while (max <= hi) max += 12; max -= 12;
						int nbPsbl = (max - min) / 12 + 1;
						if (nbPsbl < 1) nbPsbl = 1;
						int choice = Rand::integer(0, nbPsbl);
						chord[i] = min + 12 * choice;
					}
					pb = false;
					for (int i = 0; i < 4; i++) {
						if (chord[i] + 2 == chord[i + 1] && chord[i] < 53) pb = true;
					}
					if (pb) nbPbs++;
				} while (pb && nbPbs < 25);
			}
			// Classer et retourner
			Array::Sort(chord);
			return chord;
		}
		static void appendPianoChord_onBarLine(array<List<Element^>^, 1>^ out, int nbEighths, double progress, array<int, 1>^ chord1, array<int, 1>^ optionalChord2) {
			// Choisir quelle méthode appliquer
			int method = 0;
			if (nbEighths == 1) {
				method = 1; goto end;
			}
			beg: switch (Rand::integer(1, 6)) {
				case 1: method = 1; goto end;
				case 2: method = 2; goto end;
				case 3: method = 3; if (nbEighths <= 2) goto beg; goto end;
				case 4: method = 4; if (nbEighths <= 4) goto beg; goto end;
				case 5: method = 5; if (nbEighths <= 6) goto beg; goto end;
			}
			// Appliquer
			end:
			if      (method == 1) appendPianoChord_method1(out, nbEighths, chord1, false);
			else if (method == 2) appendPianoChord_method2(out, nbEighths, chord1);
			else if (method == 3) appendPianoChord_method3(out, nbEighths, chord1, Rand::double01() < 0.5);
			else if (method == 4) appendPianoChord_method4(out, nbEighths, chord1);
			else if (method == 5) appendPianoChord_method5(out, nbEighths, chord1, optionalChord2);
			else                  throw gcnew Exception;
		}
		static void appendPianoChord_straddlingBarLine(array<List<Element^>^, 1>^ out, int nbEighths, double progress, array<int, 1>^ chord1, array<int, 1>^ optionalChord2, int before, int after) {
			// Dans un cas sur deux, diviser en un cas 'entièrement dans la mesure' et un cas 'sur la mesure'
			if (Rand::double01() < 0.5) {
				appendPianoChord_entirelyInBar(out, before, progress, chord1, optionalChord2);
				appendPianoChord_onBarLine    (out, after , progress, chord1, optionalChord2);
				return;
			}
			// Choisir quelle méthode appliquer
			int method = 0;
			beg: switch (Rand::integer(1, 7)) {
				case 1: method = 1; goto end;
				case 2: method = 4; if (nbEighths <= 4 || before == 2) goto beg; goto end;
				case 3: method = 6; if (before < 2     || after < 3  ) goto beg; goto end;
				case 4: method = 7; if (before < 2     || after < 2  ) goto beg; goto end;
				case 5: method = 8; if (before < 2                   ) goto beg; goto end;
				case 6: method = 9; if (before < 3     || after < 2  ) goto beg; goto end;
			}
			// Appliquer
			end:
			if      (method == 1) appendPianoChord_method1(out, nbEighths, chord1, false);
			else if (method == 4) appendPianoChord_method4(out, nbEighths, chord1);
			else if (method == 6) appendPianoChord_method6(out, nbEighths, optionalChord2        , before, after);
			else if (method == 7) appendPianoChord_method7(out, nbEighths, chord1, optionalChord2, before, after);
			else if (method == 8) appendPianoChord_method8(out, nbEighths, chord1, optionalChord2, before, after);
			else if (method == 9) appendPianoChord_method9(out, nbEighths, chord1, optionalChord2, before, after);
			else                  throw gcnew Exception;
		}
		static void appendPianoChord_entirelyInBar(array<List<Element^>^, 1>^ out, int nbEighths, double progress, array<int, 1>^ chord1, array<int, 1>^ optionalChord2) {
			// Choisir quelle méthode appliquer
			int method = 0;
			if (nbEighths == 1) {
				method = Rand::double01() < 0.5 ? 1 : 12; goto end;
			}
			beg: switch (Rand::integer(1, 7)) {
				case 1: method =  1; goto end;
				case 2: method =  7; if (nbEighths <= 4) goto beg; goto end;
				case 3: method =  8; if (nbEighths <= 3) goto beg; goto end;
				case 4: method = 10; if (nbEighths <= 3) goto beg; goto end;
				case 5: method = 11; if (nbEighths <= 2) goto beg; goto end;
				case 6: method = 12; goto end;
			}
			// Appliquer
			end:
			if      (method ==  1) appendPianoChord_method1 (out, nbEighths, chord1, Rand::double01() < 0.8);
			else if (method ==  7) appendPianoChord_method7 (out, nbEighths, chord1, optionalChord2, 2, nbEighths - 2);
			else if (method ==  8) appendPianoChord_method8 (out, nbEighths, chord1, optionalChord2, 2, nbEighths - 2);
			else if (method == 10) appendPianoChord_method10(out, nbEighths, Rand::double01() < 0.5 ? chord1 : optionalChord2);
			else if (method == 11) appendPianoChord_method11(out, nbEighths, chord1, optionalChord2);
			else if (method == 12) appendPianoChord_method12(out, nbEighths, Rand::double01() < 0.5 ? chord1 : optionalChord2);
			else                   throw gcnew Exception;
		}
	private:
		static void appendPianoChord_method1(array<List<Element^>^, 1>^ out, int nbEighths, array<int, 1>^ chord, bool bassIsUp) {
			if (bassIsUp) {
				chord = copy(chord);
				while (chord[0] < chord[4]) {
					chord[0] += 12;
				}
				Array::Sort(chord);
			}
			array<Pitch^, 1> ^left, ^right; {
				if (chord[2] - chord[0] > chord[4] - chord[2]) {
					left  = gcnew array<Pitch^, 1>{gcnew Pitch(chord[0]), gcnew Pitch(chord[1])};
					right = gcnew array<Pitch^, 1>{gcnew Pitch(chord[2]), gcnew Pitch(chord[3]), gcnew Pitch(chord[4])};
				} else {
					left  = gcnew array<Pitch^, 1>{gcnew Pitch(chord[0]), gcnew Pitch(chord[1]), gcnew Pitch(chord[2])};
					right = gcnew array<Pitch^, 1>{gcnew Pitch(chord[3]), gcnew Pitch(chord[4])};
				}
			}
			for (int i = 0; i < nbEighths; i++) {
				bool tied = i < nbEighths - 1;
				NoteDecorations^ deco = qNoteDecorations(tied ? "_" : "");
				out[0]->Add(gcnew Note(qRhythm("e"), right, deco));
				out[1]->Add(gcnew Note(qRhythm("e"), left , deco));
			}
		}
		static void appendPianoChord_method2(array<List<Element^>^, 1>^ out, int nbEighths, array<int, 1>^ chord) {
			array<Pitch^, 1> ^left, ^right; {
				left = gcnew array<Pitch^, 1>{gcnew Pitch(chord[0]), gcnew Pitch(chord[1])};
				right = gcnew array<Pitch^, 1>{gcnew Pitch(chord[2]), gcnew Pitch(chord[3]), gcnew Pitch(chord[4])};
			}
			out[0]->Add(qRest("e"));
			out[1]->Add(gcnew Note(qRhythm("e"), left[0], qNoteDecorations("_")));
			for (int i = 1; i < nbEighths; i++) {
				bool tied = i < nbEighths - 1;
				NoteDecorations^ deco = qNoteDecorations(tied ? "_" : "");
				out[0]->Add(gcnew Note(qRhythm("e"), right, deco));
				out[1]->Add(gcnew Note(qRhythm("e"), left , deco));
			}
		}
		static void appendPianoChord_method3(array<List<Element^>^, 1>^ out, int nbEighths, array<int, 1>^ chord, bool bassIsUp) {
			if (bassIsUp) {
				chord = copy(chord);
				while (chord[0] < chord[4]) {
					chord[0] += 12;
				}
				Array::Sort(chord);
			}
			array<Pitch^, 1> ^left, ^right; {
				if (chord[2] - chord[0] > chord[4] - chord[2]) {
					left  = gcnew array<Pitch^, 1>{gcnew Pitch(chord[0]), gcnew Pitch(chord[1])};
					right = gcnew array<Pitch^, 1>{gcnew Pitch(chord[2]), gcnew Pitch(chord[3]), gcnew Pitch(chord[4])};
				} else {
					left  = gcnew array<Pitch^, 1>{gcnew Pitch(chord[0]), gcnew Pitch(chord[1]), gcnew Pitch(chord[2])};
					right = gcnew array<Pitch^, 1>{gcnew Pitch(chord[3]), gcnew Pitch(chord[4])};
				}
			}
			for (int i = 0; i < nbEighths; i++) {
				bool tied = i > 0 && i < nbEighths - 1;
				NoteDecorations^ deco = qNoteDecorations(i == 0 ? (tied ? "-_" : "-") : (tied ? "_" : ""));
				out[0]->Add(gcnew Note(qRhythm("e"), right, deco));
				out[1]->Add(gcnew Note(qRhythm("e"), left , deco));
			}
		}
		static void appendPianoChord_method4(array<List<Element^>^, 1>^ out, int nbEighths, array<int, 1>^ chord) {
			array<Pitch^, 1> ^left, ^right; {
				left = gcnew array<Pitch^, 1>{gcnew Pitch(chord[0]), gcnew Pitch(chord[1])};
				right = gcnew array<Pitch^, 1>{gcnew Pitch(chord[2]), gcnew Pitch(chord[3]), gcnew Pitch(chord[4])};
			}
			out[0]->Add(qRest("e"));
			out[0]->Add(gcnew Note(qRhythm("q"), right));
			for (int i = 3; i < nbEighths; i++) {
				bool tied = i < nbEighths - 1;
				NoteDecorations^ deco = qNoteDecorations(tied ? "_" : "");
				out[0]->Add(gcnew Note(qRhythm("e"), right, deco));
			}
			for (int i = 0; i < nbEighths; i++) {
				bool tied = i < nbEighths - 1;
				NoteDecorations^ deco = qNoteDecorations(tied ? "_" : "");
				out[1]->Add(gcnew Note(qRhythm("e"), left, deco));
			}
		}
		static void appendPianoChord_method5(array<List<Element^>^, 1>^ out, int nbEighths, array<int, 1>^ chord1, array<int, 1>^ chord2) {
			appendPianoChord_method1(out,             3, chord1, false);
			appendPianoChord_method1(out, nbEighths - 3, chord2, false);
		}
		static void appendPianoChord_method6(array<List<Element^>^, 1>^ out, int nbEighths, array<int, 1>^ chord, int before, int after) {
			appendPianoChord_method1(out, before, chord, false);
			appendPianoChord_method2(out, after, chord);
		}
		static void appendPianoChord_method7(array<List<Element^>^, 1>^ out, int nbEighths, array<int, 1>^ chord1, array<int, 1>^ chord2, int before, int after) {
			appendPianoChord_method1(out, before, chord1, false);
			appendPianoChord_method1(out, after , chord2, false);
		}
		static void appendPianoChord_method8(array<List<Element^>^, 1>^ out, int nbEighths, array<int, 1>^ chord1, array<int, 1>^ chord2, int before, int after) {
			appendPianoChord_method1(out, before, chord1, Rand::double01() < 0.5);
			for (int i = 0; i < after; i++) {
				bool tied = i < after - 1;
				NoteDecorations^ deco = qNoteDecorations(tied ? "_" : "");
				out[0]->Add(qRest("e"));
				out[1]->Add(gcnew Note(qRhythm("e"), gcnew Pitch(chord2[0]), deco));
			}
		}
		static void appendPianoChord_method9(array<List<Element^>^, 1>^ out, int nbEighths, array<int, 1>^ chord1, array<int, 1>^ chord2, int before, int after) {
			appendPianoChord_method10(out, before, chord1);
			appendPianoChord_method1 (out, after , chord2, false);
		}
		static void appendPianoChord_method10(array<List<Element^>^, 1>^ out, int nbEighths, array<int, 1>^ chord) {
			int last = -1;
			for (int i = 0; i < nbEighths; i++) {
				int pitch_int;
				do {
					pitch_int = Rand::chooseIn(chord);
				} while (pitch_int == last);
				last = pitch_int;
				int pitchToChooseVoice = pitch_int + 6;
				array<Pitch^, 1>^ pitches =gcnew array<Pitch^, 1>{gcnew Pitch(pitch_int), gcnew Pitch(pitch_int + 12)};
				bool up = pitchToChooseVoice >= 60;
				Note^ note = gcnew Note(qRhythm("e"), pitches);
				Rest^ rest = qRest("e");
				if (up) { out[0]->Add(note); out[1]->Add(rest); }
				else    { out[0]->Add(rest); out[1]->Add(note); }
			}
		}
		static void appendPianoChord_method11(array<List<Element^>^, 1>^ out, int nbEighths, array<int, 1>^ chord1, array<int, 1>^ chord2) {
			int before = nbEighths / 2;
			int after = nbEighths - before;
			appendPianoChord_method12(out, before, chord2);
			appendPianoChord_method12(out, after , chord1);
		}
		static void appendPianoChord_method12(array<List<Element^>^, 1>^ out, int nbEighths, array<int, 1>^ chord) {
			array<Pitch^, 1>^ pitches = gcnew array<Pitch^, 1>{gcnew Pitch(chord[0]), gcnew Pitch(chord[0] + 12)};
			for (int i = 0; i < nbEighths; i++) {
				bool tied = i < nbEighths - 1;
				NoteDecorations^ deco = qNoteDecorations(tied ? "_" : "");
				out[0]->Add(qRest("e"));
				out[1]->Add(gcnew Note(qRhythm("e"), pitches, deco));
			}
		}
	private:
		// Violon
		static List<Element^>^ createViolinVoice(array<array<int, 1>^, 1>^ modesSeq, array<int, 1>^ durations, int nbEighths) {
			int nbBars = nbEighths / 8;
			// Récupérer l'harmonie de chaque croche du morceau
			array<array<int, 1>^, 1>^ modeAt = getModeAt(modesSeq, durations, nbEighths);
			// Décider quelles mesures sont jouées
			array<bool, 1>^ play = decidePlayedBars(nbBars);
			// Créer chaque mesure
			List<Element^>^ ans = gcnew List<Element^>;
			Last^ lastNote = gcnew Last;
			for (int bar = 0; bar < nbBars; bar++) {
				double progress = ((double)bar) / (nbBars - 1);
				array<array<int, 1>^, 1>^ modes = gcnew array<array<int, 1>^, 1> {modeAt[8 * bar], modeAt[8 * bar + 1], modeAt[8 * bar + 2], modeAt[8 * bar + 3], modeAt[8 * bar + 4], modeAt[8 * bar + 5], modeAt[8 * bar + 6], modeAt[8 * bar + 7]};
				lastNote = appendViolinBar_returnLastNote(ans, play[bar], progress, modes, lastNote);
			}
			// Retourner
			return ans;
		}
		ref class Last {
			private:
				bool _exists;
				int  _val;
				bool _tied;
			public:
				Last() :
					_exists(false),
					_tied(false)
				{ }
				Last(int val, bool tied) :
					_exists(true),
					_val(val),
					_tied(tied)
				{ }
			public:
				property bool exists {
					bool get() {
						return _exists;
					}
				}
				property int val {
					int get() {
						if (_exists) return _val;
						else throw gcnew Exception("This instance has no value.");
					}
				}
				property bool tied {
					bool get() {
						return _tied;
					}
				}
		};
		static array<bool, 1>^ decidePlayedBars(int nbBars) {
			array<bool, 1>^ ans = gcnew array<bool, 1>(nbBars);
			double prop = 0.75;
			// Remplissage initial
			for (int i = 0; i < 3; i++) ans[i] = false;
			for (int i = 3; i < nbBars - 3; i++) ans[i] = Rand::double01() < prop;
			for (int i = nbBars - 3; i < nbBars; i++) ans[i] = true;
			// Réorganisation
			double reorg = 4.5;
			int nbReorg = (int)Math::Round(reorg * (nbBars - 6));
			for (int n = 0; n < nbReorg; n++) {
				int i = Rand::integer(3, nbBars - 7);
				bool a(ans[i]), b(ans[i + 1]), c(ans[i + 2]), d(ans[i + 3]);
				if ((a ^ b) && (b ^ c) && (c ^ d)) {
					ans[i + 1] = a;
					ans[i + 2] = d;
				}
			}
			// Retour
			return ans;
		}
		static array<array<int, 1>^, 1>^ getModeAt(array<array<int, 1>^, 1>^ modesSeq, array<int, 1>^ durations, int nbEighths) {
			array<array<int, 1>^, 1>^ ans = gcnew array<array<int, 1>^, 1>(nbEighths);
			int nbInSeq = modesSeq->Length;
			int pos = 0;
			for (int modeIndexInSeq = 0; modeIndexInSeq < nbInSeq; modeIndexInSeq++) {
				array<int, 1>^ mode = modesSeq[modeIndexInSeq];
				int dur = durations[modeIndexInSeq];
				for (int n = 0; n < dur; n++) {
					ans[pos++] = mode;
				}
			}
			for (int i = pos; i < nbEighths; i++) {
				ans[i] = ans[i - 1];
			}
			return ans;
		}
		static Last^ appendViolinBar_returnLastNote(List<Element^>^ out, bool play, double progress, array<array<int, 1>^, 1>^ modes, Last^ lastNote) {
			switch (Rand::integer(1, 11)) {
				case  1: return appendViolinBar_returnLastNote_method1 (out, play, progress, modes, lastNote);
				case  2: return appendViolinBar_returnLastNote_method2 (out, play, progress, modes, lastNote);
				case  3: return appendViolinBar_returnLastNote_method3 (out, play, progress, modes, lastNote);
				case  4: return appendViolinBar_returnLastNote_method4 (out, play, progress, modes, lastNote);
				case  5: return appendViolinBar_returnLastNote_method5 (out, play, progress, modes, lastNote);
				case  6: return appendViolinBar_returnLastNote_method6 (out, play, progress, modes, lastNote);
				case  7: return appendViolinBar_returnLastNote_method7 (out, play, progress, modes, lastNote);
				case  8: return appendViolinBar_returnLastNote_method8 (out, play, progress, modes, lastNote);
				case  9: return appendViolinBar_returnLastNote_method9 (out, play, progress, modes, lastNote);
				case 10: return appendViolinBar_returnLastNote_method10(out, play, progress, modes, lastNote);
				default: throw gcnew Exception;
			}
		}
		static Last^ appendViolinBar_returnLastNote_voidBar(List<Element^>^ out, Last^ lastNote) {
			if (lastNote->tied) {
				out->Add(gcnew Note(qRhythm("q"), gcnew Pitch(lastNote->val)));
				out->Add(qRest("h."));
			} else {
				out->Add(qRest("w"));
			}
			return gcnew Last;
		}
		static Last^ appendViolinBar_returnLastNote_method1(List<Element^>^ out, bool play, double progress, array<array<int, 1>^, 1>^ modes, Last^ lastNote) {
			// Mesure non jouée ?
			if (!play) return appendViolinBar_returnLastNote_voidBar(out, lastNote);
			// Ajouter la queue éventuelle de la note liée, sinon ajouter un silence
			if (lastNote->tied) {
				out->Add(gcnew Note(qRhythm("e"), gcnew Pitch(lastNote->val)));
				out->Add(qRest("e"));
			} else {
				out->Add(qRest("q"));
			}
			// Décider si la note sera liée à la suite
			bool tiedToNextBar = Rand::double01() < 0.5;
			// Choisir le nouveau pitch
			int pitch = (!lastNote->exists || Rand::double01() < 0.5)
			          ? randomInMode(modes[2])
			          : nearestInMode(lastNote->val + Rand::integer(-5, 6), modes[2]);
			// Ajouter la blanche pointée
			out->Add(gcnew Note(qRhythm("h."), gcnew Pitch(pitch), qNoteDecorations(tiedToNextBar ? "_" : "")));
			// Retourner
			return gcnew Last(pitch, tiedToNextBar);
		}
		static Last^ appendViolinBar_returnLastNote_method2(List<Element^>^ out, bool play, double progress, array<array<int, 1>^, 1>^ modes, Last^ lastNote) {
			// Mesure non jouée ?
			if (!play) return appendViolinBar_returnLastNote_voidBar(out, lastNote);
			// Pitch de la 1ère note
			int pitchFirstNote;
			if (lastNote->tied) pitchFirstNote = lastNote->val;
			else if (lastNote->exists) pitchFirstNote = nearestInMode(lastNote->val - 3, modes[0]);
			else pitchFirstNote = randomInMode(modes[0]);
			// Ajouter la première note
			out->Add(gcnew Note(qRhythm("q"), gcnew Pitch(pitchFirstNote)));
			// Décider si la dernière note sera liée à la suite
			bool tiedToNextBar = Rand::double01() < 0.2;
			// Choisir le nouveau pitch
			int newPitch = nearestInMode(pitchFirstNote + 7, modes[2]);
			// Ajouter la blanche pointée
			out->Add(gcnew Note(qRhythm("h."), gcnew Pitch(newPitch), qNoteDecorations(tiedToNextBar ? "_" : "")));
			// Retourner
			return gcnew Last(newPitch, tiedToNextBar);
		}
		static Last^ appendViolinBar_returnLastNote_method3(List<Element^>^ out, bool play, double progress, array<array<int, 1>^, 1>^ modes, Last^ lastNote) {
			// Mesure non jouée ?
			if (!play) return appendViolinBar_returnLastNote_voidBar(out, lastNote);
			// Ajouter la queue éventuelle de la note liée, sinon ajouter un silence
			if (lastNote->tied) {
				out->Add(gcnew Note(qRhythm("e"), gcnew Pitch(lastNote->val)));
				out->Add(qRest("e"));
			} else {
				out->Add(qRest("q"));
			}
			// Décider si la dernière note sera liée à la suite
			bool tiedToNextBar = Rand::double01() < 0.65;
			// Choisir les nouveaux pitchs
			int pitch1 = (!lastNote->exists || Rand::double01() < 0.25)
			           ? randomInMode(modes[2])
			           : nearestInMode(lastNote->val + Rand::integer(-8, -3), modes[2]);
			int pitch2 = nearestInMode(pitch1 + 3, modes[4]);
			int pitch3 = nearestInMode(pitch2 + 4, modes[6]);
			// Ajouter le contenu
			out->Add(gcnew Note(qRhythm("q"), gcnew Pitch(pitch1)));
			out->Add(gcnew Note(qRhythm("q"), gcnew Pitch(pitch2)));
			out->Add(gcnew Note(qRhythm("q"), gcnew Pitch(pitch3), qNoteDecorations(tiedToNextBar ? "_" : "")));
			// Retourner
			return gcnew Last(pitch3, tiedToNextBar);
		}
		static Last^ appendViolinBar_returnLastNote_method4(List<Element^>^ out, bool play, double progress, array<array<int, 1>^, 1>^ modes, Last^ lastNote) {
			// Mesure non jouée ?
			if (!play) return appendViolinBar_returnLastNote_voidBar(out, lastNote);
			// Pitch de la 1ère note
			int pitch1;
			if (lastNote->tied) pitch1 = lastNote->val;
			else if (lastNote->exists) pitch1 = nearestInMode(lastNote->val + 4, modes[0]);
			else pitch1 = randomInMode(modes[0]);
			// Choisir les nouveaux pitchs
			int pitch2 = nearestInMode(pitch1 - 3, modes[2]);
			int pitch3 = nearestInMode(pitch2 + (Rand::double01() < 0.5 ? 4 : -3), modes[4]);
			int pitch4 = nearestInMode(pitch3 - 4, modes[6]);
			// Décider si la dernière note sera liée à la suite
			bool tiedToNextBar = Rand::double01() < 0.4;
			// Ajouter les noires
			out->Add(gcnew Note(qRhythm("q"), gcnew Pitch(pitch1)));
			out->Add(gcnew Note(qRhythm("q"), gcnew Pitch(pitch2)));
			out->Add(gcnew Note(qRhythm("q"), gcnew Pitch(pitch3)));
			out->Add(gcnew Note(qRhythm("q"), gcnew Pitch(pitch4), qNoteDecorations(tiedToNextBar ? "_" : "")));
			// Retourner
			return gcnew Last(pitch4, tiedToNextBar);
		}
		static Last^ appendViolinBar_returnLastNote_method5(List<Element^>^ out, bool play, double progress, array<array<int, 1>^, 1>^ modes, Last^ lastNote) {
			// Mesure non jouée ?
			if (!play) return appendViolinBar_returnLastNote_voidBar(out, lastNote);
			// Pitch de la 1ère note
			int pitch1;
			if (lastNote->tied) pitch1 = lastNote->val;
			else if (lastNote->exists) pitch1 = nearestInMode(lastNote->val + 4, modes[0]);
			else pitch1 = randomInMode(modes[0]);
			// Choisir les nouveaux pitchs
			int pitch2 = nearestInMode(pitch1 - 2, modes[0]);
			int pitch3 = nearestInMode(pitch2 - 2, modes[1]);
			int pitch4 = nearestInMode(pitch3 + 3, modes[2]);
			int pitch5 = nearestInMode(pitch4 - 6, modes[3]);
			int pitch6 = nearestInMode(pitch5 + 4, modes[4]);
			int pitch7 = nearestInMode(pitch6 - 8, modes[7]);
			// Décider si la dernière note sera liée à la suite
			bool tiedToNextBar = Rand::double01() < 0.4;
			// Ajouter les notes
			out->Add(qElement("[3:2 e|" + pitch1 + " e|" + pitch2 + " e|" + pitch3 + "]"));
			out->Add(qElement("e|" + pitch4));
			out->Add(qElement("e|" + pitch5));
			out->Add(qElement("q.|" + pitch6));
			out->Add(gcnew Note(qRhythm("e"), gcnew Pitch(pitch7), qNoteDecorations(tiedToNextBar ? "_" : "")));
			// Retourner
			return gcnew Last(pitch7, tiedToNextBar);
		}
		static Last^ appendViolinBar_returnLastNote_method6(List<Element^>^ out, bool play, double progress, array<array<int, 1>^, 1>^ modes, Last^ lastNote) {
			// Mesure non jouée ?
			if (!play) return appendViolinBar_returnLastNote_voidBar(out, lastNote);
			// Ajouter la queue éventuelle de la note liée, sinon ajouter un silence
			if (lastNote->tied) {
				out->Add(gcnew Note(qRhythm("e"), gcnew Pitch(lastNote->val)));
				out->Add(qRest("e"));
			} else {
				out->Add(qRest("q"));
			}
			// Choisir les nouveaux pitchs
			int pitch1 = (!lastNote->exists || Rand::double01() < 0.6)
			           ? randomInMode(modes[2])
			           : nearestInMode(lastNote->val + Rand::integer(1, 7), modes[2]);
			int pitch2 = nearestInMode(pitch1 - 2, modes[4]);
			int pitch3 = nearestInMode(pitch2 - 2, modes[5]);
			int pitch4 = nearestInMode(pitch3 - 3, modes[6]);
			int pitch5 = nearestInMode(pitch4 - 2, modes[6]);
			int pitch6 = nearestInMode(pitch5 - 3, modes[7]);
			// Décider si la dernière note sera liée à la suite
			bool tiedToNextBar = Rand::double01() < 0.65;
			// Ajouter le contenu
			out->Add(qElement("q_|" + pitch1));
			out->Add(qElement("[3:2 e|" + pitch1 + " e|" + pitch2 + " e|" + pitch3 + "]"));
			out->Add(qElement("[3:2 e|" + pitch4 + " e|" + pitch5 + " e" + (tiedToNextBar ? "_" : "") + "|" + pitch6 + "]"));
			// Retourner
			return gcnew Last(pitch6, tiedToNextBar);
		}
		static Last^ appendViolinBar_returnLastNote_method7(List<Element^>^ out, bool play, double progress, array<array<int, 1>^, 1>^ modes, Last^ lastNote) {
			// Mesure non jouée ?
			if (!play) return appendViolinBar_returnLastNote_voidBar(out, lastNote);
			// Pitch de la 1ère note
			int pitch1;
			if (lastNote->tied) pitch1 = lastNote->val;
			else if (lastNote->exists) pitch1 = nearestInMode(lastNote->val + 4, modes[0]);
			else pitch1 = randomInMode(modes[0]);
			// Choisir le nouveau pitch
			int pitch2 = nearestInMode(pitch1 - 9, modes[4]);
			// Décider si la dernière note sera liée à la suite
			bool tiedToNextBar = Rand::double01() < 0.8;
			// Ajouter les éléments
			out->Add(gcnew Note(qRhythm("h"), gcnew Pitch(pitch1)));
			out->Add(gcnew Note(qRhythm("h"), gcnew Pitch(pitch2), qNoteDecorations(tiedToNextBar ? "_" : "")));
			// Retourner
			return gcnew Last(pitch2, tiedToNextBar);
		}
		static Last^ appendViolinBar_returnLastNote_method8(List<Element^>^ out, bool play, double progress, array<array<int, 1>^, 1>^ modes, Last^ lastNote) {
			// Mesure non jouée ?
			if (!play) return appendViolinBar_returnLastNote_voidBar(out, lastNote);
			// Pitch de la 1ère note
			int pitch1;
			if (lastNote->tied) pitch1 = lastNote->val;
			else if (lastNote->exists) pitch1 = nearestInMode(lastNote->val + 4, modes[0]);
			else pitch1 = randomInMode(modes[0]);
			// Choisir les nouveaux pitchs
			int pitch2 = nearestInMode(pitch1 + 2, modes[1]);
			int pitch3 = nearestInMode(pitch2 + 5, modes[2]);
			int pitch4 = nearestInMode(pitch3 - 2, modes[4]);
			int pitch5 = nearestInMode(pitch4 + 5, modes[5]);
			int pitch6 = nearestInMode(pitch5 + 3, modes[6]);
			// Décider si la dernière note sera liée à la suite
			bool tiedToNextBar = Rand::double01() < 0.8;
			// Ajouter les éléments
			out->Add(qElement("[3:2 q|" + pitch1 + " q|" + pitch2 + " q|" + pitch3 + "]"));
			out->Add(qElement("[3:2 q|" + pitch4 + " q|" + pitch5 + " q" + (tiedToNextBar ? "_" : "") + "|" + pitch6 + "]"));
			// Retourner
			return gcnew Last(pitch6, tiedToNextBar);
		}
		static Last^ appendViolinBar_returnLastNote_method9(List<Element^>^ out, bool play, double progress, array<array<int, 1>^, 1>^ modes, Last^ lastNote) {
			// Mesure non jouée ?
			if (!play) return appendViolinBar_returnLastNote_voidBar(out, lastNote);
			// Ajouter la queue éventuelle de la note liée, sinon ajouter un silence
			if (lastNote->tied) {
				out->Add(gcnew Note(qRhythm("q"), gcnew Pitch(lastNote->val)));
				out->Add(qRest("q"));
			} else {
				out->Add(qRest("h"));
			}
			// Décider si la note sera liée à la suite
			bool tiedToNextBar = Rand::double01() < 0.6;
			// Choisir le nouveau pitch
			int pitch = (!lastNote->exists || Rand::double01() < 0.5)
			          ? randomInMode(modes[4])
			          : nearestInMode(lastNote->val + Rand::integer(-5, 6), modes[4]);
			// Ajouter la blanche
			out->Add(gcnew Note(qRhythm("h"), gcnew Pitch(pitch), qNoteDecorations(tiedToNextBar ? "_" : "")));
			// Retourner
			return gcnew Last(pitch, tiedToNextBar);
		}
		static Last^ appendViolinBar_returnLastNote_method10(List<Element^>^ out, bool play, double progress, array<array<int, 1>^, 1>^ modes, Last^ lastNote) {
			// Mesure non jouée ?
			if (!play) return appendViolinBar_returnLastNote_voidBar(out, lastNote);
			// Pitch de la 1ère note
			int pitch1;
			if (lastNote->tied) pitch1 = lastNote->val;
			else if (lastNote->exists) pitch1 = nearestInMode(lastNote->val + 4, modes[0]);
			else pitch1 = randomInMode(modes[0]);
			// Décider si la note sera liée à la suite
			bool tiedToNextBar = Rand::double01() < 0.8;
			// Ajouter la ronde
			out->Add(gcnew Note(qRhythm("w"), gcnew Pitch(pitch1), qNoteDecorations(tiedToNextBar ? "_" : "")));
			// Retourner
			return gcnew Last(pitch1, tiedToNextBar);
		}
	private:
		static int randomInMode(array<int, 1>^ mode) {
			return nearestInMode(Rand::integer(60, 83), mode);
		}
		static int nearestInMode(int note, array<int, 1>^ mode) {
			if      (isInMode(note    , mode)) return note;
			else if (isInMode(note + 1, mode)) return note + 1;
			else if (isInMode(note - 1, mode)) return note - 1;
			else                               throw;
		}
		static bool isInMode(int note, array<int, 1>^ mode) {
			int proj = note % 12;
			for each (int noteInMode in mode) if (proj == noteInMode) return true;
			return false;
		}
};

// Accélération
public ref class G_Accel : public Generator {
	private:
		static const int nbNotes = 40;
		static const double tempoInit = 60.;
	public:
		virtual String^ name() override { return "Accélération"; }
		virtual String^ description() override { return "Notes uniformément accélérées, pour construction de rythmes d'accélération infinie."; }
		virtual ScoreAndInfos^ generate() override {
			// Créer une liste de noires
			array<Note^, 1>^ notes = gcnew array<Note^, 1>(nbNotes + 1);
			for (int i = 0; i <= nbNotes; i++) {
				notes[i] = qNote("q|c4<°q=" + getTempoAt(i) + ">");
			}
			// Créer le bloc
			Block^ block = gcnew Block(1);
			block->addBars(nbNotes + 1, qSignature("1/4"));
			// Ajouter les éléments
			for (int i = 0; i <= nbNotes; i++) {
				block[1, i + 1] = gcnew Group(notes[i]);
			}
			// Retourner
			return block;
		}
	private:
		static String^ getTempoAt(int i) {
			double progress = ((double)i) / ((double)nbNotes);
			double val = tempoInit * Math::Pow(2, progress);
			double round = Math::Round(val, 2);
			//return ("" + round)->Replace(",", ".");;
			return "" + round;
		}
};

// Invention chute de pizzicati
public ref class G_InventionChutePizzicati : public Generator {
	private:
		ref struct Range {
			public:
				int min;
				int max;
			public:
				Range(int min, int max) :
					min(min),
					max(max)
				{ }
		};
	private:
		//static const int nbLastNotesExclude = 4;
		//static array<int, 1>^ preferredIntervals = gcnew array<int, 1>{1, 2, 3, 4, 6, 8, 9, 10, 11};
		//static const int nbTries = 2000;
	public:
		virtual String^ name() override { return "Invention chute de pizzicati"; }
		virtual String^ description() override { return "Pizz. de violoncelle, en trois chutes, pour l'invention."; }
		virtual ScoreAndInfos^ generate() override {
			// Générer un 'Group' pour chaque descente (trois au total)
			Group^ group1 = generateFall(1, 67, 51, 53, 45);
			Group^ group2 = generateFall(2, 67, 56, 59, 53);
			Group^ group3 = generateFall(3, 68, 46, 50, 37);
			// Générer un 'Group' séparateur
			Group^ sep1 = qGroup("q<.pizz.> q q q q");
			Group^ sep2 = qGroup("q q q q q");
			// Retourner les groupes concaténés
			return gcnew Block(sep1 + group1 + sep2 + group2 + sep2 + group3 + sep2);
		}
	private:
		static Group^ generateFall(int fallIndex, int topBeg, int bottomBeg, int topEnd, int bottomEnd) {
			// Créer les plages de notes : le haut est un chromatisme cassé, le bas descend linéairement
			array<Range^, 1>^ ranges = generateRanges(topBeg, bottomBeg, topEnd, bottomEnd);
			// Répéter certaines plages
			ranges = repeatSomeRanges(ranges, fallIndex);
			int nbRanges = ranges->Length;
			// Créer les nombres de notes par plage
			array<int, 1>^ nbNotesInEachRange = generateNbNotesInEachRange(fallIndex, nbRanges);




				// PROV
				// Concaténer tous les ambitus sous forme d'une suite de noires
				List<Element^>^ elements = gcnew List<Element^>;
				for (int i = 0; i < nbRanges; i++) {
					Range^ range = ranges[i];
					String^ text = "<." + nbNotesInEachRange[i] + ">";
					String^ chordDescr = "q|" + range->min + "|" + range->max + text;
					elements->Add(qNote(chordDescr));
				}
				// Retourner
				return gcnew Group(elements);

			//// Créer les hauteurs de notes pour chaque plage
			//array<List<int>^, 1>^ pitches = generatePitches(fallIndex, ranges, nbRanges);
			//// Concaténer toutes les séquences sous forme d'une suite de doubles croches
			//List<Element^>^ elements = gcnew List<Element^>;
			//for each (List<int>^ pitchesList in pitches) {
			//	for each (int pitch in pitchesList) {
			//		elements->Add(qNote("16th|" + pitch));
			//	}
			//}
			//// Retourner
			//return gcnew Group(elements);
		}
		static array<Range^, 1>^ generateRanges(int topBeg, int bottomBeg, int topEnd, int bottomEnd) {
			int nbRanges = topBeg - topEnd + 1;
			// Créer les hauts et les bas
			array<int, 1>^ rangeTops    = generateRangeTops   (topBeg   , topEnd   , nbRanges);
			array<int, 1>^ rangeBottoms = generateRangeBottoms(bottomBeg, bottomEnd, nbRanges);
			// Créer les plages
			array<Range^, 1>^ ans = gcnew array<Range^, 1>(nbRanges);
			for (int i = 0; i < nbRanges; i++) {
				ans[i] = gcnew Range(rangeBottoms[i], rangeTops[i]);
			}
			// Retourner
			return ans;
		}
		static array<int, 1>^ generateRangeTops(int beg, int end, int nb) {
			// Créer les variables
			array<int, 1>^ ans = gcnew array<int, 1>(nb);
			// Remplir le tableau linéairement (descente chromatique)
			for (int i = 0; i < nb; i++) {
				ans[i] = beg - i;
			}
			// Opérer un certain nombre d'échanges pour obtenir un chromatisme cassé
			const double exchangeRatio = 2.5;
			int nbExchanges = (int)Math::Round(exchangeRatio * (nb - 3));
			for (int n = 0; n < nbExchanges; n++) {
				int pos1 = Rand::integer(1, nb - 2);
				int pos2 = pos1 + 1;
				int exchg = ans[pos1];
				ans[pos1] = ans[pos2];
				ans[pos2] = exchg;
			}
			// Retourner
			return ans;
		}
		static array<int, 1>^ generateRangeBottoms(int beg, int end, int nb) {
			// Créer les variables
			array<int, 1>^ ans = gcnew array<int, 1>(nb);
			double beg_more = beg + 0.499999;
			double end_more = end - 0.499999;
			// Remplir le tableau linéairement
			for (int i = 0; i < nb; i++) {
				double progress = ((double)i) / (nb - 1);
				ans[i] = (int)Math::Round(beg_more + (end_more - beg_more) * progress);
			}
			// Retourner
			return ans;
		}
		static array<int, 1>^ generateNbNotesInEachRange(int fallIndex, int nbRanges) {
			array<int, 1>^ ans = gcnew array<int, 1>(nbRanges);
			for (int i = 0; i < nbRanges; i++) {
				double progress = ((double)i) / (nbRanges - 1);
				ans[i] = getPreferredNbPitchesInThisRange(fallIndex, progress);
			}
			return ans;
		}
		static array<Range^, 1>^ repeatSomeRanges(array<Range^, 1>^ ranges, int fallIndex) {
			int nb = ranges->Length;
			List<Range^>^ ans = gcnew List<Range^>;
			for (int i = 0; i < nb; i++) {
				double progress = ((double)i) / (nb - 1);
				int nbRep = getNbRepetitions(fallIndex, progress);
				for (int n = 0; n < nbRep; n++) {
					ans->Add(ranges[i]);
				}
			}
			return ans->ToArray();
		}
		static int getNbRepetitions(int fallIndex, double progress) {
			if (fallIndex == 1) {
				// 1 ou 2
				return Rand::integer(1, 3);
			} else if (fallIndex == 2) {
				// Entre 1 et 3
				return Rand::integer(1, 4);
			} else if (fallIndex == 3) {
				// 1
				return 1;
			} else {
				throw gcnew Exception;
			}
		}

		
//		static array<List<int>^, 1>^ generatePitches(int fallIndex, array<Range^, 1>^ ranges, int nbRanges) {
//			array<List<int>^, 1>^ ans = gcnew array<List<int>^, 1>(nbRanges);
//			for (int i = 0; i < nbRanges; i++) {
//				// Déterminer les hauteurs à exclure ('nbLastNotesExclude' dernières notes générées, note supérieure du groupe suivant)
//				List<int>^ excludePitches = getExcludePitches(i, ans, ranges, nbRanges);
//				// Générer les hauteurs de notes
//				ans[i] = generatePitchesInRange(ranges[i], fallIndex, ((double)i) / (nbRanges - 1), excludePitches);
//			}
//			return ans;
//		}
//		static List<int>^ getExcludePitches(int currentIndex, array<List<int>^, 1>^ generated, array<Range^, 1>^ ranges, int nbRanges) {
//			// Créer la liste
//			List<int>^ ans = gcnew List<int>;
//			// Ajouter (si possible) la note supérieure du groupe suivant, qui est à exclure
//			if (currentIndex < nbRanges - 1) ans->Add(ranges[currentIndex + 1]->max);
//			// Ajouter (si possible) les 'nbLastNotesExclude' dernières notes générées, qui sont à exclure
//			int indexInLastGenerated = currentIndex;
//			int nbLastNotesExcluded = 0;
//			while (true) {
//				indexInLastGenerated--;
//				if (indexInLastGenerated < 0) break;
//				array<int, 1>^ pitchesAtThisIndex = generated[indexInLastGenerated]->ToArray();
//				int nbPitchesAtThisIndex = pitchesAtThisIndex->Length;
//				for (int i = nbPitchesAtThisIndex - 1; i >= 0; i--) {
//					ans->Add(pitchesAtThisIndex[i]);
//					nbLastNotesExcluded++;
//					if (nbLastNotesExcluded == nbLastNotesExclude) {
//						break;
//					}
//				}
//				if (nbLastNotesExcluded == nbLastNotesExclude) {
//					break;
//				}
//			}
//			// Retourner
//			return ans;
//		}
//		static List<int>^ generatePitchesInRange(Range^ range, int fallIndex, double progressInFall, List<int>^ excludePitches) {
//			// Déterminer le nombre de notes souhaité
//			int preferredNbNotes = getPreferredNbPitchesInThisRange(fallIndex, progressInFall);
//			// Lister les hauteurs autorisées (en ordre croissant)
//			array<int, 1>^ possiblePitches = getPossiblePitches(range, excludePitches);
//			// Générer les hauteurs de notes
//			List<int>^ pitches = generatePitchesAmongPossible_usingPreferredIntervals(possiblePitches, preferredNbNotes);
//			// Eventuellement, opérer des inversions internes
//			pitches = doInversionsInSeq_ifNecessary(pitches, fallIndex, progressInFall);
//			// Eventuellement, inverser globalement
//			if (fallIndex == 2) pitches->Reverse();
//			// Retourner
//			return pitches;
//		}
		static int getPreferredNbPitchesInThisRange(int fallIndex, double progressInFall) {
			switch (fallIndex) {
				case 1: {
					// Entre 2 et 5
					return Rand::integer(2, 6);
				}
				case 2: {
					// Au début, entre 2 et 4 ; à la fin, entre 1 et 2
					double begMax = 4;
					double begMin = 2;
					double endMax = 2;
					double endMin = 1;
					int nowMax = (int)Math::Round((begMax + 0.49999) + ((endMax - 0.49999) - (begMax + 0.49999)) * progressInFall);
					int nowMin = (int)Math::Round((begMin + 0.49999) + ((endMin - 0.49999) - (begMin + 0.49999)) * progressInFall);
					return Rand::integer(nowMin, nowMax + 1);
				}
				case 3: {
					// Entre 1 et 6
					return Rand::integer(1, 7);
				}
				default: {
					throw gcnew Exception;
				}
			}
		}
//		static array<int, 1>^ getPossiblePitches(Range^ range, List<int>^ excludePitches) {
//			List<int>^ ans = gcnew List<int>;
//			for (int pitch = range->min; pitch <= range->max; pitch++) {
//				bool isInExcludePitches = excludePitches->Contains(pitch);
//				bool isSameAsTopPitchButOtherOctave = pitch != range->max && (range->max - pitch + 1200) % 12 == 0;
//				if (!isInExcludePitches && !isSameAsTopPitchButOtherOctave) {
//					ans->Add(pitch);
//				}
//			}
//			return ans->ToArray();
//		}
//		static List<int>^ generatePitchesAmongPossible_usingPreferredIntervals(array<int, 1>^ possiblePitches, int preferredNbNotes) {
//			int nbPossiblePitches = possiblePitches->Length;
//			// S'il n'y a pas assez (ou s'il y a exactement assez) de hauteurs possibles, les retourner toutes
//			if (nbPossiblePitches <= preferredNbNotes) {
//				List<int>^ ans = gcnew List<int>;
//				for (int i = nbPossiblePitches - 1; i >= 0; i--) {
//					ans->Add(possiblePitches[i]);
//				}
//				return ans;
//			}
//			// Déterminer l'ambitus des hauteurs possibles
//			int possibleAmbitus = possiblePitches[nbPossiblePitches - 1] - possiblePitches[0];
//			// Générer un grand nombre de séquences et choisir la meilleure
//			List<int>^ sequenceBestScore;
//			double bestScore01 = -1;
//			List<int>^ currentSequence;
//			double currentSequenceScore01;
//			for (int n = 0; n < nbTries; n++) {
//				// Créer une séquence et lui attribuer un score entre 0 et 1 (en fonction des intervalles préférés, et un peu de l'ambitus couvert)
//				currentSequence = generateRandomDescendingSequenceInPossiblePitches(possiblePitches, nbPossiblePitches, preferredNbNotes);
//				currentSequenceScore01 = calcSequenceScore01_consideringPreferredIntervalsAndAmbitus(currentSequence->ToArray(), possibleAmbitus);
//				// Si le score est 1, retourner directement cette séquence optimale
//				if (currentSequenceScore01 == 1) return currentSequence;
//				// Sinon, la conserver si elle est meilleure que la meilleure précédente
//				if (currentSequenceScore01 > bestScore01) {
//					sequenceBestScore = currentSequence;
//					bestScore01 = currentSequenceScore01;
//				}
//			}
//			// Si aucune séquence optimale n'a encore été retournée, retourner la meilleure conservée
//			return sequenceBestScore;
//		}
//		static List<int>^ doInversionsInSeq_ifNecessary(List<int>^ seq, int fallIndex, double progressInFall) {
//			int len = seq->Count;
//			// Déterminer si l'on souhaite faire des inversions
//			bool doInversions; {
//				if (len <= 2) {
//					doInversions = false;
//				} else {
//					double proba;
//					if (fallIndex == 1) {
//						// probabilité d'inversion : 0.05 --> 0.35
//						proba = 0.05 + 0.3 * progressInFall;
//					} else if (fallIndex == 2) {
//						// probabilité d'inversion : 0.5
//						proba = 0.5;
//					} else if (fallIndex == 3) {
//						// probabilité d'inversion : 0 --> 0.35 puis subitement 0
//						if (progressInFall <= 0.5) proba = 0.7 * progressInFall;
//						else proba = 0;
//					} else {
//						throw gcnew Exception;
//					}
//					doInversions = Rand::double01() < proba;
//				}
//			}
//			// Inverser ?
//			if (doInversions) {
//				// Déterminer le nombre d'inversions à effectuer (maximum 2)
//				int nbInversions; {
//					if (len <= 4) nbInversions = 1;
//					else nbInversions = Rand::integer(1, 3);
//				}
//				// Effectuer le bon nombre d'inversions (ne jamais inverser la première note, ne pas faire deux fois la même inversion)
//				array<int, 1>^ seq_arr = seq->ToArray(); {
//					int index1, index2;
//					int last_index1 = -1;
//					for (int n = 0; n < nbInversions; n++) {
//						// Choisir la position de l'inversion
//						do { index1 = Rand::integer(1, len - 1); } while (index1 == last_index1);
//						last_index1 = index1;
//						index2 = index1 + 1;
//						// Inverser
//						int exchanger__ = seq_arr[index1];
//						seq_arr[index1] = seq_arr[index2];
//						seq_arr[index2] = exchanger__;
//					}
//				}
//				// Retourner
//				return gcnew List<int>(seq_arr);
//			} else {
//				// Pas d'inversion : retourner la séquence initiale
//				return seq;
//			}
//		}
//		static List<int>^ generateRandomDescendingSequenceInPossiblePitches(array<int, 1>^ possiblePitches, int nbPossiblePitches, int nbNotes) {
//			// Positionner 'true' sur 'nbNotes' indices différents (dont l'indice 'nbPossiblePitches - 1')
//			array<bool, 1>^ keep; {
//				keep = gcnew array<bool, 1>(nbPossiblePitches);
//				keep[nbPossiblePitches - 1] = true;
//				int nbTrued = 1;
//				while (nbTrued < nbNotes) {
//					int index = Rand::integer(nbPossiblePitches);
//					if (!keep[index]) {
//						keep[index] = true;
//						nbTrued++;
//					}
//				}
//			}
//			// Créer la séquence descendante
//			List<int>^ ans = gcnew List<int>; {
//				for (int i = nbPossiblePitches - 1; i >= 0; i--) {
//					if (keep[i]) {
//						ans->Add(possiblePitches[i]);
//					}
//				}
//			}
//			// Retourner
//			return ans;
//		}
//		static double calcSequenceScore01_consideringPreferredIntervalsAndAmbitus(array<int, 1>^ seq, int possibleAmbitus) {
//			int len = seq->Length;
//			int nbInterv = len - 1;
//			// Séquence d'un seul élément : toujours le meilleur score
//			if (len <= 1) return 1;
//			// Calculer le score de bons intervalles (entre 0 et 1)
//			double intervScore; {
//				// Score entre -nbInterv et 2 * nbInterv : les octaves et grand intervalles valent -1, les bons valent 2, les autres 0
//				int scoreIntervInt = 0;
//				for (int i = 0; i < nbInterv; i++) {
//					scoreIntervInt += getIntervScore(seq[i], seq[i + 1]);
//				}
//				// Normalisation entre 0 et 1
//				intervScore = (double)(scoreIntervInt + nbInterv) / (3 * nbInterv);
//			}
//			// Calculer la couverture d'ambitus (entre 0 et 1 : le max est atteint aléatoirement 2 fois sur 3 ou si l'ambitus couvre l'ambitus disponible)
//			double ambitusScore; {
//				if (Rand::integer(3) <= 1) {
//					ambitusScore = 1;
//				} else {
//					ambitusScore = ((double)Math::Abs(seq[len - 1] - seq[0])) / possibleAmbitus;
//				}
//			}
//			// Calculer le total (les intervalles comptent pour 85% dans le résultat final)
//			double totalScore; {
//				double ratioInterv = 0.85;
//				totalScore = ratioInterv * intervScore + (1 - ratioInterv) * ambitusScore;
//				if (totalScore > 0.99999) totalScore = 1;
//			}
//			// Retourner
//			return totalScore;
//		}
//		static int getIntervScore(int pitch1, int pitch2) {
//			int interv = Math::Abs(pitch2 - pitch1);
//			if (interv >= 12) {
//				// Octave ou grand intervalle : très mauvais
//				return -1;
//			} else if (contains(preferredIntervals, interv)) {
//				// Intervalle préféré : très bon
//				return 2;
//			} else {
//				// Intervalle non préféré
//				return 0;
//			}
//		}
};

// Quizz Métaclassique
public ref class G_QuizzMetaclassique : public Generator {

	// Privés
	private:
		static const int n_or = 10;
		static const int nbNotes = 72;
		static const int timeOffset = 22;
		static const bool mirrorPitches = false;

		static const int maxIntervTop = 3;
		static const int minPitchTop      = 52;
		static const int initialPitchTop1 = 64;
		static const int initialPitchTop2 = 54;
		static const int maxPitchTop      = 78;

		static const int minPitchBottom = 30;
	private:
		ref struct CustomNote {
			int instant;
			int duration;
			int pitch;
			CustomNote(int instant, int duration, int pitch) :
				instant(instant),
				duration(duration),
				pitch(pitch)
			{ }
			CustomNote(CustomNote^ other) :
				instant(other->instant),
				duration(other->duration),
				pitch(other->pitch)
			{ }
		};
	private:
		int getRandomDurTopVoice() {
			if (Rand::double01() < 0.27) {
				// Grace note
				return 0;
			} else if (Rand::double01() < 0.88) {
				// Normal
				return Rand::integer(1, 12);
			} else {
				// Long
				return Rand::integer(30, 49);
			}
		}
		array<int, 1>^ createDurations() {
			array<int, 1>^ ans = gcnew array<int, 1>(nbNotes);
			for (int i = 0; i < nbNotes; i++) {
				ans[i] = getRandomDurTopVoice();
			}
			return ans;
		}
		array<int, 1>^ createInstantsTop(array<int, 1>^ durations) {
			array<int, 1>^ ans = gcnew array<int, 1>(nbNotes);
			ans[0] = 0;
			for (int i = 1; i < nbNotes; i++) {
				ans[i] = ans[i - 1] + durations[i - 1];
			}
			return ans;
		}
		int calcTotalDurTop(array<int, 1>^ durations) {
			int ans = 0;
			for (int i = 0; i < nbNotes; i++) {
				ans += durations[i];
			}
			return ans;
		}
		int getNextPitchTop(array<int, 1>^ prev, int i) {
			int ans;
			do {
				// Nouvelle hauteur
				int blah = Rand::integer(0, 2);
				if (blah == 0) ans = prev[i - 2];
				else if (blah == 1) ans = prev[i - 3];
				int interv = Rand::integer(-maxIntervTop, maxIntervTop + 1);
				ans += interv;
				// Correct ?
				if (ans >= minPitchTop && ans <= maxPitchTop) break;
			} while (true);
			return ans;
		}
		array<int, 1>^ createPitchesTop() {
			array<int, 1>^ ans = gcnew array<int, 1>(nbNotes);
			ans[0] = initialPitchTop1;
			ans[1] = initialPitchTop1;
			ans[2] = initialPitchTop2;
			for (int i = 3; i < nbNotes; i++) {
				ans[i] = getNextPitchTop(ans, i);
			}
			return ans;
		}
	private:
		array<int, 1>^ createInstantsBottom(array<int, 1>^ instantsTop) {
			array<int, 1>^ ans = gcnew array<int, 1>(nbNotes);
			for (int i = 0; i < nbNotes; i++) {
				ans[i] = instantsTop[i] + timeOffset;
			}
			return ans;
		}
		array<int, 1>^ createPitchesBottom(array<int, 1>^ pitchesTop) {
			// Copier, éventuellement en miroir
			array<int, 1>^ ans = gcnew array<int, 1>(nbNotes);
			for (int i = 0; i < nbNotes; i++) {
				if (mirrorPitches) ans[i] = -pitchesTop[i];
				else ans[i] = pitchesTop[i];
			}
			// Déterminer la hauteur min atteinte
			int currentMin = ans[0];
			for each (int pitch in ans) if (pitch < currentMin) currentMin = pitch;
			// Déterminer la transposition souhaitée
			int transp = currentMin - minPitchBottom;
			// Sans miroir, éviter certains intervalles (octave, quarte, quinte)
			if (!mirrorPitches) while (transp % 12 == 0 || transp % 12 == 5 || transp % 12 == 7) transp++;
			// Transposer
			for (int i = 0; i < nbNotes; i++) {
				ans[i] -= transp;
			}
			// Retourner
			return ans;
		}
	private:
		array<int, 1>^ getPitchesTopChord(int basePitch) {
			array<int, 1>^ ans = gcnew array<int, 1> {0, 4, 8, 10, 13, 15, 17, 18, 19, 21};
			if (ans->Length != n_or) throw gcnew Exception; // assertion
			for (int i = 0; i < n_or; i++) ans[i] += basePitch;
			return ans;
		}
		void fillTopVoice_oneNote_oneBar_oneSubVoice(Block^ block, List<int>^ subVoiceGraceNotesOrNull, int subVoicePitch, int barBase1, bool isFirst, bool isLast, int voiceIndexBase1) {
			List<Element^>^ elements = gcnew List<Element^>;
			NoteDecorations^ deco;
			Note^ nextNote;
			// Ajouter les appoggiatures
			bool hasGraceNotes = subVoiceGraceNotesOrNull != nullptr && subVoiceGraceNotesOrNull->Count > 0;
			if (hasGraceNotes) {
				bool isFirstGraceNote = true;
				for each (int pitch in subVoiceGraceNotesOrNull) {
					bool slurBeg = isFirstGraceNote;
					deco = gcnew NoteDecorations(0, slurBeg, false);
					nextNote = gcnew Note(Rhythm::graceRhythm(RhythmFigure::_32nd), gcnew Pitch(pitch), deco);
					elements->Add(nextNote);
					isFirstGraceNote = false;
				}
			}
			// Ajouter la note principale
			bool tied = !isLast;
			bool slurEnd = hasGraceNotes;
			array<Articulation^, 1>^ articulations = isFirst ? newArray(Articulation::accent) : newArray<Articulation^>();
			deco = gcnew NoteDecorations(articulations, tied, false, slurEnd);
			nextNote = gcnew Note(RhythmFigure::_16th, gcnew Pitch(subVoicePitch), deco);
			elements->Add(nextNote);
			// Ajouter les éléments au bloc
			block[voiceIndexBase1, barBase1] = gcnew Group(elements);
		}
		List<int>^ selectGraceNotesSubVoiceTop(List<int>^ graceNotesOrNull, int i) {
			if (graceNotesOrNull == nullptr) {
				return nullptr;
			} else {
				List<int>^ ans = gcnew List<int>;
				for each (int basePitch in graceNotesOrNull) {
					ans->Add(getPitchesTopChord(basePitch)[i]);
				}
				return ans;
			}
		}
		void fillTopVoice_oneNote_oneBar(Block^ block, List<int>^ graceNotesOrNull, array<int, 1>^ pitchesMainNote, int instant, bool isFirst, bool isLast) {
			for (int i = 0; i < n_or; i++) {
				fillTopVoice_oneNote_oneBar_oneSubVoice(
					block,
					selectGraceNotesSubVoiceTop(graceNotesOrNull, i),
					pitchesMainNote[i],
					instant + 1,
					isFirst,
					isLast,
					n_or - i
				);
			}
		}
		void fillTopVoice_oneNote(Block^ block, List<int>^ graceNotes, CustomNote^ current) {
			array<int, 1>^ pitchesMainNote = getPitchesTopChord(current->pitch);
			for (int i = 0; i < current->duration; i++) {
				fillTopVoice_oneNote_oneBar(
					block,
					(i == 0) ? graceNotes : nullptr,
					pitchesMainNote,
					current->instant + i,
					i == 0,
					i == current->duration - 1
				);
			}
		}
		void fillTopVoice(Block^ block, array<int, 1>^ instantsTop, array<int, 1>^ durations, array<int, 1>^ pitchesTop) {
			List<int>^ graceNotesBuffer = gcnew List<int>;
			for (int i = 0; i < nbNotes; i++) {
				CustomNote^ current = gcnew CustomNote(instantsTop[i], durations[i], pitchesTop[i]);
				if (durations[i] == 0) {
					graceNotesBuffer->Add(current->pitch);
				} else {
					fillTopVoice_oneNote(block, graceNotesBuffer, current);
					graceNotesBuffer->Clear();
				}
			}
		}
	private:
		array<int, 1>^ getPitchesBottomChord(int basePitch) {
			array<int, 1>^ ans = gcnew array<int, 1> {0, 4, 8, 10, 13, 15, 17, 18, 19, 21};
			if (ans->Length != n_or) throw gcnew Exception; // assertion
			for (int i = 0; i < n_or; i++) ans[i] += basePitch;
			return ans;
		}
		void fillBottomVoice_oneNote_oneBar_oneSubVoice(Block^ block, List<int>^ subVoiceGraceNotesOrNull, int subVoicePitch, int barBase1, bool isFirst, bool isLast, int voiceIndexBase1) {
			List<Element^>^ elements = gcnew List<Element^>;
			NoteDecorations^ deco;
			Note^ nextNote;
			// Ajouter les appoggiatures
			bool hasGraceNotes = subVoiceGraceNotesOrNull != nullptr && subVoiceGraceNotesOrNull->Count > 0;
			if (hasGraceNotes) {
				bool isFirstGraceNote = true;
				for each (int pitch in subVoiceGraceNotesOrNull) {
					bool slurBeg = isFirstGraceNote;
					deco = gcnew NoteDecorations(0, slurBeg, false);
					nextNote = gcnew Note(Rhythm::graceRhythm(RhythmFigure::_32nd), gcnew Pitch(pitch), deco);
					elements->Add(nextNote);
					isFirstGraceNote = false;
				}
			}
			// Ajouter la note principale
			bool tied = !isLast;
			bool slurEnd = hasGraceNotes;
			array<Articulation^, 1>^ articulations = isFirst ? newArray(Articulation::accent) : newArray<Articulation^>();
			deco = gcnew NoteDecorations(articulations, tied, false, slurEnd);
			nextNote = gcnew Note(RhythmFigure::_16th, gcnew Pitch(subVoicePitch), deco);
			elements->Add(nextNote);
			// Ajouter les éléments au bloc
			block[voiceIndexBase1, barBase1] = gcnew Group(elements);
		}
		List<int>^ selectGraceNotesSubVoiceBottom(List<int>^ graceNotesOrNull, int i) {
			if (graceNotesOrNull == nullptr) {
				return nullptr;
			} else {
				List<int>^ ans = gcnew List<int>;
				for each (int basePitch in graceNotesOrNull) {
					ans->Add(getPitchesBottomChord(basePitch)[i]);
				}
				return ans;
			}
		}
		void fillBottomVoice_oneNote_oneBar(Block^ block, List<int>^ graceNotesOrNull, array<int, 1>^ pitchesMainNote, int instant, bool isFirst, bool isLast) {
			for (int i = 0; i < n_or; i++) {
				fillBottomVoice_oneNote_oneBar_oneSubVoice(
					block,
					selectGraceNotesSubVoiceBottom(graceNotesOrNull, i),
					pitchesMainNote[i],
					instant + 1,
					isFirst,
					isLast,
					2 * n_or - i
				);
			}
		}
		void fillBottomVoice_oneNote(Block^ block, List<int>^ graceNotes, CustomNote^ current) {
			array<int, 1>^ pitchesMainNote = getPitchesBottomChord(current->pitch);
			for (int i = 0; i < current->duration; i++) {
				fillBottomVoice_oneNote_oneBar(
					block,
					(i == 0) ? graceNotes : nullptr,
					pitchesMainNote,
					current->instant + i,
					i == 0,
					i == current->duration - 1
				);
			}
		}
		void fillBottomVoice(Block^ block, array<int, 1>^ instantsBottom, array<int, 1>^ durations, array<int, 1>^ pitchesBottom) {
			List<int>^ graceNotesBuffer = gcnew List<int>;
			for (int i = 0; i < nbNotes; i++) {
				CustomNote^ current = gcnew CustomNote(instantsBottom[i], durations[i], pitchesBottom[i]);
				if (durations[i] == 0) {
					graceNotesBuffer->Add(current->pitch);
				} else {
					fillBottomVoice_oneNote(block, graceNotesBuffer, current);
					graceNotesBuffer->Clear();
				}
			}
		}

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Quizz pour Métaclassique";

		}
		virtual String^ description() override {
			
			return "Système de contraintes sur piano de grand-mère.";

		}
		virtual ScoreAndInfos^ generate() override {
			Rand::randomize(0);
			// Créer les durées des notes
			array<int, 1>^ durations = createDurations();
			// Créer les instants de la voix aiguë
			array<int, 1>^ instantsTop = createInstantsTop(durations);
			int totalDurTop = calcTotalDurTop(durations);
			// Créer les instants de la voix grave
			array<int, 1>^ instantsBottom = createInstantsBottom(instantsTop);
			int totalDur = totalDurTop + timeOffset;
			// Créer les notes de la voix aiguë
			array<int, 1>^ pitchesTop = createPitchesTop();
			// Créer les notes de la voix grave
			array<int, 1>^ pitchesBottom = createPitchesBottom(pitchesTop);
			// Créer le bloc
			Block^ block = gcnew Block(2 * n_or);
			for (int i = 0; i < totalDur; i++) block->addBar(qSignature("1/16"));
			// Remplir la voix aiguë (n_or portées)
			fillTopVoice(block, instantsTop, durations, pitchesTop);
			// Remplir la voix grave (n_or portées)
			fillBottomVoice(block, instantsBottom, durations, pitchesBottom);
			// Retourner
			return block;
		}

};
