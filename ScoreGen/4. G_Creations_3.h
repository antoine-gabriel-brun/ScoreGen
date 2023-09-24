#pragma region head

	#pragma once

	#include "ScoreGenLib.h"
	using namespace ScoreGenLib;
	using namespace ScoreGenLib::Generators;
	using namespace CloudMaker;

	#include "OF_SerialWithInterv.h"

#pragma endregion

// Philharmonie : "Counterpunch"
public ref class G_Cloches : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Counterpunch : Cloches (suite sans cubes)";

		}
		virtual String^ description() override {
			
			return "Cloches réalisant une suite sans cubes sur trois notes.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			int nb = 80;
			array<int, 1>^ pitches3 = gcnew array<int, 1>{ 57, 66, 72 };
			int dur_16th_min = 4, dur_16th_max = 7;
			bool fillAll16th = false;

			// Créer la suite sans triples des hauteurs
			array<Pitch^, 1>^ pitches; {
				array<int, 1>^ squareFree = Seq::squareFreeWord3(nb);
				pitches = gcnew array<Pitch^, 1>(nb);
				for (int i = 0; i < nb; i++) {
					pitches[i] = gcnew Pitch(pitches3[squareFree[i]]);
				}
			}

			// Créer la suite de Thue-Morse des durées (comptées en doubles)
			array<int, 1>^ durations_16th;
			int totalDuration_1024th = 0;
			{
				array<bool, 1>^ thueMorse = Seq::thueMorse(nb);
				durations_16th = gcnew array<int, 1>(nb);
				for (int i = 0; i < nb; i++) {
					durations_16th[i] = thueMorse[i] ? dur_16th_max : dur_16th_min;
					totalDuration_1024th += 64 * durations_16th[i];
				}
			}

			// Créer les éléments
			List<Element^>^ elements = gcnew List<Element^>; {
				for (int i = 0; i < nb; i++) {
					Pitch^ pitch = pitches[i];
					int dur_16th = durations_16th[i];
					if (fillAll16th) {
						for (int j = 0; j < dur_16th; j++) {
							Words::Words^ words;
							if      (j == 0) words = gcnew Words::Dynamic("f");
							else if (j == 1) words = gcnew Words::Dynamic("p");
							else             words = nullptr;
							elements->Add(gcnew Note(RhythmFigure::_16th, pitch, words));
						}
					} else {
						for each (Note^ note in Note::getSum(pitch, 64 * dur_16th)) {
							elements->Add(note);
						}
					}
				}
			}

			// Créer le bloc
			Block^ block = gcnew Block(1);
			block->addBar(gcnew Signature(totalDuration_1024th, nullptr));

			// Ajouter la mesure unique
			block[1, 1] = gcnew Group(elements);

			// Retourner
			return block;

		}

};
public ref class G_SequenceFinale : public Generator {

	// Options
	private:
		static int nb = 300;
		static int nbParts = 7, part_gls = 1, part_wbl = 3, part_grc = 4, part_pn1 = 5, part_pn2 = 6, part_pn3 = 7;

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Counterpunch : Séquence finale";

		}
		virtual String^ description() override {
			
			return "Séquence 0 1 0 2 1 0 3 2 1 0 4, réalisée pour glockenspiel, wood blocks, grosse caisse et piano.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			Rand::randomize(0);

			// Créer la séquence principale, sur le bon mode
			array<int, 1>^ seq; {
				array<int, 1>^ sseq = Seq::fractal010210(nb + 3);
				seq = gcnew array<int, 1>(nb);
				for (int i = 0; i < nb; i++) seq[i] = sseq[i + 3];
				array<int, 1>^ mode = gcnew array<int, 1>{ 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 109 };
				for (int i = 0; i < nb; i++) seq[i] = mode[seq[i]];
			}

			// Déterminer les notes de la séquence jouées par le glockenspiel et par la m. d. du piano
			array<bool, 1>^ seqPlayedByGls = gcnew array<bool, 1>(nb);
			array<bool, 1>^ seqPlayedByPn1 = gcnew array<bool, 1>(nb);
			{
				bool currentIsGls = false;
				seqPlayedByGls[0] = true;
				seqPlayedByPn1[0] = true;
				for (int i = 1; i < nb; i++) {
					if (currentIsGls) seqPlayedByGls[i] = true; else seqPlayedByPn1[i] = true;
					bool isPeak = (i != nb - 1) && seq[i] > seq[i - 1] && seq[i] > seq[i + 1];
					bool shouldChange = isPeak;
					double prop = ((double)i) / (nb - 1);
					if (prop > 0.5) {
						prop = 2 * prop - 1;
						if (Rand::double01() < prop * 0.2) shouldChange = !shouldChange;
					}
					if (shouldChange) {
						currentIsGls = !currentIsGls;
						if (currentIsGls) seqPlayedByGls[i] = true; else seqPlayedByPn1[i] = true;
					}
				}
				// Relire en ajoutant des redondances
				for (int i = 2; i < nb - 1; i++) {
					bool lastIsPeak = seq[i - 1] > seq[i - 2] && seq[i - 1] > seq[i];
					double prop = ((double)(i - 1)) / (nb - 3);
					prop = prop * 1.5 - 0.5;
					if (lastIsPeak && Rand::double01() < prop * 0.65) {
						seqPlayedByGls[i] = true;
						seqPlayedByPn1[i] = true;
					}
				}

			}
			
			// Déterminer les positions et les types des insertions rythmiques de wood blocks
			array<int/* 0 = rien, 1-2-3-4 = types */, 1>^ wblInsertions = gcnew array<int, 1>(nb); {
				int beg = (int)(nb * 0.28);
				int end = nb - 1;
				// Positionner les insertions (les marquer par des 1)
				{
					int pos = beg;
					int offsetMin, offsetMax, offset;
					TimeVariable_Lin^ mmax = gcnew TimeVariable_Lin(20, 9);
					TimeVariable_Lin^ mmin = gcnew TimeVariable_Lin(15, 4);
					while (pos <= end) {
						wblInsertions[pos] = 1;
						double prop = (double(pos - beg)) / (end - beg);
						offsetMax = (int)Math::Round(mmax->at(prop));
						offsetMin = (int)Math::Round(mmin->at(prop));
						offset = Rand::integer(offsetMin, offsetMax + 1);
						pos += offset;
					}
				}
				// Choisir les types d'insertions (remplacer les 1 par des 1-2-3-4)
				{
					TimeVariable_Lin^ type1 = gcnew TimeVariable_Lin(100, 0);
					TimeVariable_Lin^ type2 = gcnew TimeVariable_Lin(-40, 80);
					TimeVariable_Lin^ type3 = gcnew TimeVariable_Lin(-120, 80);
					for (int i = beg; i <= end; i++) {
						if (wblInsertions[i] > 0) {
							double prop = ((double)(i - beg)) / (end - beg);
							double _1 = type1->at(prop); if (_1 < 0) _1 = 0;
							double _2 = type2->at(prop); if (_2 < 0) _2 = 0;
							double _3 = type3->at(prop); if (_3 < 0) _3 = 0;
							double sum = _1 + _2 + _3;
							_1 /= sum;
							_2 /= sum;
							_3 /= sum;
							double rand = Rand::double01();
							if      (rand < _1     ) wblInsertions[i] = 1;
							else if (rand < _1 + _2) wblInsertions[i] = 2;
							else                     wblInsertions[i] = 3;
						}
					}
					// Insérer un unique 4
					{
						int ppos = beg + (int)(0.85 * (end - beg));
						int offset = 0;
						while (ppos + offset > end || wblInsertions[ppos + offset] == 0) {
							if (offset > 0) offset = -offset;
							else            offset = -offset + 1;
						}
						wblInsertions[ppos + offset] = 4;
					}
				}
			}

			// Déterminer la durée de chaque position (en doubles croches), et la durée totale
			array<int, 1>^ durations;
			int totalDuration;
			{
				durations = gcnew array<int, 1>(nb);
				totalDuration = 0;
				for (int i = 0; i < nb; i++) {
					if      (wblInsertions[i] == 3) durations[i] = 2;
					else if (wblInsertions[i] == 4) durations[i] = 11;
					else                            durations[i] = 1;
					totalDuration += durations[i];
				}
			}

			// Positionner les coups de grosse caisse
			array<bool, 1>^ grc = gcnew array<bool, 1>(nb); {
				int beg = (int)(nb * 0.54);
				int end = nb - 1;
				int pos = beg;
				int offsetMin, offsetMax, offset;
				TimeVariable_Lin^ mmax = gcnew TimeVariable_Lin(15, 6);
				TimeVariable_Lin^ mmin = gcnew TimeVariable_Lin(9, 3);
				while (pos <= end) {
					grc[pos] = true;
					if (pos > beg + 0.6 * (end - beg) && pos < end && Rand::double01() < 0.5) grc[pos + 1] = true;
					double prop = (double(pos - beg)) / (end - beg);
					offsetMax = (int)Math::Round(mmax->at(prop));
					offsetMin = (int)Math::Round(mmin->at(prop));
					offset = Rand::integer(offsetMin, offsetMax + 1);
					pos += offset;
				}
			}

			// Placer les notes dissonantes de la m. g. du piano
			array<array<int, 1>^, 1>^ pn2 = gcnew array<array<int, 1>^, 1>(nb); {
				Rand::randomize(0);
				for (int i = 0; i < nb; i++) {
					double prop = ((double)i) / (nb - 1);
					double proba;
					if      (prop < 0.3333333333333) proba = 1;
					else if (prop < 0.6666666666667) proba = 2 - prop * 3;
					else                             proba = 0;
					proba *= 0.25;
					if (Rand::double01() < proba) {
						// Ajouter une note dissonante
						int r = Rand::integer(0, 4);
						if      (r == 0) /* 7ème M inférieure */ pn2[i] = newArray(             seq[i] - 11);
						else if (r == 1) /* 9ème m inférieure */ pn2[i] = newArray(seq[i] - 13             );
						else             /* Les deux          */ pn2[i] = newArray(seq[i] - 13, seq[i] - 11);
					}
				}
			}

			// Placer le rythme obstiné de la m. g. du piano
			array<bool, 1>^ pn3 = gcnew array<bool, 1>(nb); {
				// Déterminer le nombre de répétition idéal pour chaque séquence
				int rep = Math::Max(1, (int)Math::Round(0.7 * nb) / 102);
				// Déterminer la position de départ
				int pos = nb - 102 * rep;
				// Ajouter
				for (int dur = 4; dur >= 1; dur--) {
					for (int i = 0; i < rep; i++) {
						pn3[pos] = true; pos += dur; pn3[pos] = true; pos += dur + 1;
						pn3[pos] = true; pos += dur; pn3[pos] = true; pos += dur; pn3[pos] = true; pos += dur + 1;
						pn3[pos] = true; pos += dur; pn3[pos] = true; pos += dur; pn3[pos] = true; pos += dur; pn3[pos] = true; pos += dur + 1;
					}
				}
			}

			// Créer la partition comme une unique grande mesure
			Block^ block = gcnew Block(nbParts);
			block->addBar(totalDuration + "/16");
			
			// Ajouter les parties
			/* Glockenspiel et piano 1 */ {
				List<Element^>^ elements_gls = gcnew List<Element^>;
				List<Element^>^ elements_pn1 = gcnew List<Element^>;
				for (int i = 0; i < nb; i++) {
					int dur = durations[i];
					// Glockenspiel
					if (seqPlayedByGls[i]) elements_gls->Add(gcnew Note(RhythmFigure::_16th, gcnew Pitch(seq[i])));
					else                   elements_gls->Add(gcnew Rest(RhythmFigure::_16th));
					for (int j = 1; j < dur; j++) elements_gls->Add(gcnew Rest(RhythmFigure::_16th));
					// M. d. du piano
					if (seqPlayedByPn1[i]) elements_pn1->Add(gcnew Note(RhythmFigure::_16th, gcnew Pitch(seq[i] - 24)));
					else                   elements_pn1->Add(gcnew Rest(RhythmFigure::_16th));
					for (int j = 1; j < dur; j++) elements_pn1->Add(gcnew Rest(RhythmFigure::_16th));
				}
				block[part_gls, 1] = gcnew Group(elements_gls);
				block[part_pn1, 1] = gcnew Group(elements_pn1);
			}
			/* Wood block */ {
				List<Element^>^ elements = gcnew List<Element^>;
				Pitch^ p1 = gcnew Pitch(2, 4);
				Pitch^ p2 = gcnew Pitch(3, 4);
				Pitch^ p3 = gcnew Pitch(4, 4);
				for (int i = 0; i < nb; i++) {
					int t = wblInsertions[i];
					int t_ = (i > 0) ? wblInsertions[i - 1] : 0;
					if (t == 1 || t_ == 2) {
						elements->Add(gcnew Rest(RhythmFigure::_32nd));
						elements->Add(gcnew Note(RhythmFigure::_32nd, p3, Articulation::accent));
					} else if (t == 2) {
						elements->Add(gcnew Rest(RhythmFigure::_32nd));
						elements->Add(gcnew Note(RhythmFigure::_32nd, p3, Articulation::accent));
					} else if (t == 0) {
						elements->Add(gcnew Rest(RhythmFigure::_16th));
					} else if (t == 3) {
						elements->Add(gcnew Tuplet(
							gcnew TupletRatio(RhythmFigure::_16th, 3, 2),
							gcnew array<NoteOrRest^, 1> {
								gcnew Rest(RhythmFigure::_16th),
								gcnew Note(RhythmFigure::_16th, p1, Articulation::accent),
								gcnew Note(RhythmFigure::_16th, p3, Articulation::accent)
							}
						));
					} else if (t == 4) {
						int dur = durations[i];
						elements->Add(gcnew Rest(RhythmFigure::_16th, qWords(".IMPRO")));
						for (int j = 1; j < dur; j++) {
							elements->Add(gcnew Rest(RhythmFigure::_16th));
						}
					}
				}
				block[part_wbl, 1] = gcnew Group(elements);
			}
			/* Grosse caisse */ {
				List<Element^>^ elements = gcnew List<Element^>;
				Pitch^ p = gcnew Pitch(1, 1);
				for (int i = 0; i < nb; i++) {
					int dur = durations[i];
					if (grc[i]) elements->Add(gcnew Note(RhythmFigure::_16th, p));
					else        elements->Add(gcnew Rest(RhythmFigure::_16th));
					for (int j = 1; j < dur; j++) elements->Add(gcnew Rest(RhythmFigure::_16th));
				}
				block[part_grc, 1] = gcnew Group(elements);
			}
			/* Piano 2 */ {
				List<Element^>^ elements = gcnew List<Element^>;
				for (int i = 0; i < nb; i++) {
					int dur = durations[i];
					if (pn2[i] == nullptr) {
						elements->Add(gcnew Rest(RhythmFigure::_16th));
					} else {
						int nbPitches = pn2[i]->Length;
						array<Pitch^, 1>^ pitches = gcnew array<Pitch^, 1>(nbPitches);
						for (int j = 0; j < nbPitches; j++) pitches[j] = gcnew Pitch(pn2[i][j]);
						elements->Add(gcnew Note(RhythmFigure::_16th, pitches, Articulation::staccato));
					}
					for (int j = 1; j < dur; j++) elements->Add(gcnew Rest(RhythmFigure::_16th));
				}
				block[part_pn2, 1] = gcnew Group(elements);
			}
			/* Piano 3 */ {
				List<Element^>^ elements = gcnew List<Element^>;
				array<Pitch^, 1>^ pitches = newArray(gcnew Pitch(33), gcnew Pitch(34), gcnew Pitch(35), gcnew Pitch(36));
				for (int i = 0; i < nb; i++) {
					int dur = durations[i];
					if (pn3[i]) elements->Add(gcnew Note(RhythmFigure::_16th, pitches, Articulation::staccato));
					else        elements->Add(gcnew Rest(RhythmFigure::_16th));
					for (int j = 1; j < dur; j++) elements->Add(gcnew Rest(RhythmFigure::_16th));
				}
				block[part_pn3, 1] = gcnew Group(elements);
			}

			// Retourner
			return block;

		}

};

// Etudes pour piano
public ref class G_PianoStudies1 : public Generator {

	// Types
	private:
		ref struct RangeInGroupElements {
		
			public:
				int beg;
				int end;
		
			public:
				RangeInGroupElements(int beg, int end) :
					beg(beg),
					end(end)
				{ }

		};

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Etudes pour piano : 1. Sériel";

		}
		virtual String^ description() override {
			
			return "Musique basée sur une série de 12 notes choisie aléatoirement.";

		}
		virtual ScoreAndInfos^ generate() override {

			Rand::randomize(0);

			// Choisir aléatoirement une série de 12 sons
			array<int, 1>^ serie;
			int serieLen;
			{
				serieLen = 12;
				serie = gcnew array<int, 1>(serieLen);
				for (int i = 0; i < serieLen; i++) serie[i] = i;
				Rand::shuffle(serie);
			}

			// Les répartir à la bonne octave avec un dessin /\/\/\/\/\/
			{
				serie[0] += 60;
				for (int i = 1; i < serieLen; i++) {
					// Placer cette note juste au-dessus de la note précédente
					int note_upper = serie[i];
					while (note_upper < serie[i - 1]) note_upper += 12;
					// Choisir
					if (i % 2 == 1) serie[i] = note_upper;
					else            serie[i] = note_upper - 12;
				}
			}

			// Déterminer les durées
			array<int, 1>^ durations; {
				durations = gcnew array<int, 1>{ 0, 0, 1, 1, 1, 1, 2, 3, 4, 5, 6, 7 };
				Rand::shuffle(durations);
				{
					int i = serieLen - 1;
					while (durations[i] == 0) {
						int j = Rand::integer(0, serieLen - 1);
						int exchange = durations[i];
						durations[i] = durations[j];
						durations[j] = exchange;
					}
				}
			}

			// Extraire les éléments
			array<array<Element^, 1>^, 1>^ groupElements; {
				groupElements = gcnew array<array<Element^, 1>^, 1>(serieLen);
				for (int i = 0; i < serieLen; i++) {
					array<Pitch^, 1>^ pitch = gcnew array<Pitch^, 1>{ gcnew Pitch(serie[i]) };
					int dur = durations[i];
					if (dur == 0) {
						groupElements[i] = Note::graceNote(RhythmFigure::_16th, pitch);
					} else {
						for (int n = 0; n < dur; n++) {
							groupElements[i] = Note::getSum(pitch, dur * 128);
						}
					}
				}
			}

			// Etablir l'ordre de lecture
			List<RangeInGroupElements^>^ ranges = gcnew List<RangeInGroupElements^>;
			ranges->Add(gcnew RangeInGroupElements(0, serieLen - 1));
			for (int len = 1; len <= serieLen - 1; len++) ranges->Add(gcnew RangeInGroupElements(serieLen - len, serieLen - 1));

			// Extraire les éléments dans cet ordre de lecture
			List<array<Element^, 1>^>^ groupElements_playBack; {
				groupElements_playBack = gcnew List<array<Element^, 1>^>;
				for each (RangeInGroupElements^ range in ranges) {
					for (int i = range->beg; i <= range->end; i++) {
						groupElements_playBack->Add(groupElements[i]);
					}
				}
			}

			// Transposer de plus en plus au cours du morceau (un peu vers le bas et surtout vers le haut)
			{
				List<array<Element^, 1>^>^ new_groupElements_playBack = gcnew List<array<Element^, 1>^>;
				int nb = groupElements_playBack->Count;
				int i = 0;
				const double transp_min_beg = 0.4;
				const double transp_max_beg = 0.6;
				const double transp_min_end = -1.4;
				const double transp_max_end = 2.9;
				for each (array<Element^, 1>^ group in groupElements_playBack) {
					double t01 = ((double)i) / (nb - 1);
					// Choisir la transposition
					int transp; {
						double transpMin = transp_min_beg + t01 * (transp_min_end - transp_min_beg);
						double transpMax = transp_max_beg + t01 * (transp_max_end - transp_max_beg);
						transp = 12 * (int)Math::Floor(transpMin + Rand::double01() * (transpMax - transpMin));
					}
					// Transposer
					{
						int groupLen = group->Length;
						array<Element^, 1>^ newGroup = gcnew array<Element^, 1>(groupLen);
						for (int j = 0; j < groupLen; j++) {
							Note^ note = (Note^)(group[j]);
							note = gcnew Note(note, gcnew array<Pitch^, 1>{ note->pitches[0] + (double)transp });
							newGroup[j] = note;
						}
						new_groupElements_playBack->Add(newGroup);
					}
					// Incrémenter
					i++;
				}
				groupElements_playBack = new_groupElements_playBack;
			}

			// Déterminer la durée
			int dur_8th; {
				dur_8th = 0;
				for each (array<Element^, 1>^ groupElement in groupElements_playBack) {
					for each (Element^ element in groupElement) {
						dur_8th += element->duration_1024th;
					}
				}
				dur_8th /= 128;
			}

			// Placer temporellement la main gauche
			array<int, 1>^ times_left_8th;
			int nbLeft;
			{
				List<int>^ times_left_8th_lst = gcnew List<int>;
				int start = dur_8th / 12;
				int current = start;
				while (true) {
					double t01 = ((double)(current - start)) / (dur_8th - 1 - start);
					double max = 12 - 9 * t01;
					double min = 8 - 5 * t01;
					int incr = (int)Math::Round(Rand::double01() * (max - min) + min);
					times_left_8th_lst->Add(current);
					current += incr;
					if (current >= dur_8th) break;
				}
				times_left_8th = times_left_8th_lst->ToArray();
				nbLeft = times_left_8th->Length;
			}

			// Déterminer les notes de la main gauche
			array<int, 1>^ notes_left; {
				notes_left = gcnew array<int, 1>(nbLeft);
				notes_left[0] = 59;
				// Descente chromatique...
				for (int i = 1; i < nbLeft; i++) notes_left[i] = notes_left[i - 1] - 1;
				// ...avec des échanges locaux
				for (int n = 0; n < nbLeft; n++) {
					int i = Rand::integer(0, nbLeft - 1);
					int j = i + 1;
					int exchange_ = notes_left[i];
					notes_left[i] = notes_left[j];
					notes_left[j] = exchange_;
				}
			}

			// Créer le groupe de la main droite
			Group^ group_r; {
				List<Element^>^ elements = gcnew List<Element^>;
				for each (array<Element^, 1>^ groupElement in groupElements_playBack) {
					for each (Element^ element in groupElement) {
						elements->Add(element);
					}
				}
				group_r = gcnew Group(elements);
			}

			// Créer le groupe de la main gauche
			Group^ group_l; {
				List<Element^>^ elements = gcnew List<Element^>;
				for each (Rest^ rest in Rest::getSum(times_left_8th[0] * 128)) {
					elements->Add(rest);
				}
				for (int i = 0; i < nbLeft; i++) {
					int durThisNote_8th = (i < nbLeft - 1) ? (times_left_8th[i + 1] - times_left_8th[i]) : (dur_8th - times_left_8th[i]);
					for each (Note^ note in Note::getSum(gcnew array<Pitch^, 1>{ gcnew Pitch(notes_left[i]) }, durThisNote_8th * 128)) {
						elements->Add(note);
					}
				}
				group_l = gcnew Group(elements);
			}

			// Créer et retourner le block
			Block^ block;
			block = gcnew Block(2);
			block->addBar(dur_8th + "/8");
			block[1, 1] = group_r;
			block[2, 1] = group_l;
			return block;

		}

};

// "Transcriptions" Margaux
public ref class G_SoupeTonale : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Soupe tonale";

		}
		virtual String^ description() override {
			
			return "Génère une soupe tonale de densité croissante, qui ajoute progressivement des dièses sur un mode de ré.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			int nbEighths = 240;

			// Créer le CloudMaker
			auto cloudMaker = createCloudMaker(nbEighths);

			// Générer le nuage
			array<PatternWithPos^, 1>^ cloud = cloudMaker->createCloud();
			int nbElementsInCloud = cloud->Length;

			// Modifier le contenu du nuage
			Random^ rd = gcnew Random(0);
			for (int i = 0; i < nbElementsInCloud; i++) cloud[i] = gcnew PatternWithPos(modifyElement(cloud[i]->pattern, cloud[i]->pos_1024th / 128, nbEighths, rd), cloud[i]->pos_1024th);

			// Convertir le nuage en block et retourner
			return Generators::CloudMaker::CloudMaker::cloudToBlock(cloud);

		}

	// Fonctions
	Generators::CloudMaker::CloudMaker^ createCloudMaker(int nbEighths) {
		
		Generators::CloudMaker::CloudMaker^ ans = gcnew Generators::CloudMaker::CloudMaker(
			nbEighths,
			RhythmFigure::_eighth,
			gcnew TimeVariable_Exp(0.8, 7)
		);
		
		ans->addPattern(gcnew PatternWithProba(
			qElements("q|E4|A4 h|D4|B4"),
			PatternPossiblePosition::onlyOnEighth,
			gcnew TimeVariable_Const(1)
		));
		
		ans->addPattern(gcnew PatternWithProba(
			qElements("q.|F4|C5 e_|G4|A4 h|G4|A4"),
			PatternPossiblePosition::onlyOnEighth,
			gcnew TimeVariable_Exp(0.01, 1)
		));

		return ans;

	}
	Pattern^ modifyElement(Pattern^ pattern, int pos, int nbEighths, Random^ rd) {
	
		return gcnew Pattern(modifyElements(pattern->elements, pos, nbEighths, rd), pattern->possiblePosition);

	}
	array<Element^, 1>^ modifyElements(array<Element^, 1>^ elements, int pos, int nbEighths, Random^ rd) {
		
		int nb = elements->Length;
		array<Element^, 1>^ ans = gcnew array<Element^, 1>(nb);

		// Avancement
		double time = Math::Min(1., Math::Max(0., ((double)pos) / ((double)nbEighths)));

		// Recopier en appliquant une octaviation
		int oct; {
			double t1 = 0.4;
			double t2 = 0.62;
			double t3 = 0.72;
			int octMin, octMax;
			if      (time < t1) octMin =  0;
			else if (time < t3) octMin = -1;
			else                octMin = -2;
			if      (time < t2) octMax =  0;
			else                octMax =  1;
			oct = rd->Next(octMin, octMax + 1);
		}
		Transformations::Transformation^ trans = gcnew Transformations::Transposition(12 * oct);
		for (int i = 0; i < nb; i++) {
			Note^ note = (Note^)(elements[i]);
			array<Pitch^, 1>^ pitches = note->pitches;
			int nbPitches = pitches->Length;
			for (int j = 0; j < nbPitches; j++) {
				pitches[j] = pitches[j]->transform(trans);
			}
			ans[i] = gcnew Note(note, pitches);
		}

		// Appliquer le mode à chaque hauteur
		int nbDiesis; {
			int cpt = 0;
			if      (time < ((double)(cpt += 17)) / 81) nbDiesis = 0;
			else if (time < ((double)(cpt += 15)) / 81) nbDiesis = 1;
			else if (time < ((double)(cpt += 13)) / 81) nbDiesis = 2;
			else if (time < ((double)(cpt += 11)) / 81) nbDiesis = 3;
			else if (time < ((double)(cpt +=  9)) / 81) nbDiesis = 4;
			else if (time < ((double)(cpt +=  7)) / 81) nbDiesis = 5;
			else if (time < ((double)(cpt +=  5)) / 81) nbDiesis = 6;
			else if (time < ((double)(cpt +=  3)) / 81) nbDiesis = 7;
			else if (time < ((double)(cpt +=  1)) / 81) nbDiesis = 7; // que jusqu'à 7
		}
		for (int i = 0; i < nb; i++) {
			Note^ note = (Note^)(ans[i]);
			array<Pitch^, 1>^ pitches = note->pitches;
			int nbPitches = pitches->Length;
			for (int j = 0; j < nbPitches; j++) {
				pitches[j] = fitToScale(pitches[j], nbDiesis);
			}
			ans[i] = gcnew Note(note, pitches);
		}

		// Retourner
		return ans;

	}
	Pitch^ fitToScale(Pitch^ pitch, int nbDiesis) {
		
		char step = pitch->step;
		int add;

		if      (step == 'F') add = (nbDiesis <= 0) ? 0 : 1;
		else if (step == 'C') add = (nbDiesis <= 1) ? 0 : 1;
		else if (step == 'G') add = (nbDiesis <= 2) ? 0 : 1;
		else if (step == 'D') add = (nbDiesis <= 3) ? 0 : 1;
		else if (step == 'A') add = (nbDiesis <= 4) ? 0 : 1;
		else if (step == 'E') add = (nbDiesis <= 5) ? 0 : 1;
		else if (step == 'B') add = (nbDiesis <= 6) ? 0 : 1;

		return gcnew Pitch((int)(pitch->midiValue) + add);

	}

};

// EOC
public ref class G_EOC_Rythmique : public Generator {

	// Options
	private:
		static const int nbBars = 86;
	private:
		static const int nbInstr = 13;
		static const int p_hb  = 1;
		static const int p_cl  = 2;
		static const int p_tpt = 3;
		static const int p_tbn = 4;
		static const int p_tom = 5;
		static const int p_grc = 6;
		static const int p_vib = 7;
		static const int p_pno = 8;
		static const int p_vl  = 10;
		static const int p_al  = 11;
		static const int p_vc  = 12;
		static const int p_cb  = 13;

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "EOC : Partie rythmique";

		}
		virtual String^ description() override {
			
			return "Passage rythmique.";

		}
		virtual ScoreAndInfos^ generate() override {

			Rand::randomize(0);

			// Générer séquence de mesures, avancements pour chaque type de mesure, durées en triples croches, instrumentations
			array<int, 1>^ seq = getNewBarsSeq(); for (int i = 0; i < 4; i++) seq[i] = 0;
			array<double, 1>^ avancements = getBarsAvancements(seq);
			array<int, 1>^ durations = getDurations(seq, avancements);
			array<int, 2>^ instrPlays_nothing_rhythm_melody = getInstrPlays_nothing_rhythm_melody(seq);

			// Générer la mélodie, pour la durée complète sauf -le début -les breaks
			int barMelodyBegin = 4;
			int melodyLength_eighths = getMelodyLength_eighths(barMelodyBegin, seq, durations);
			array<array<int, 1>^, 1>^ melody_asChordsIndices = getMelody_asChordsIndices(melodyLength_eighths);

			// Créer le bloc et ajouter les mesures une à une
			Block^ block = gcnew Block(nbInstr);
			int posInMelody = -1;
			for (int bar = 0; bar < nbBars; bar++) {
				// Ajouter la mesure
				addNewBarToBlock(
					block,
					bar,
					seq[bar],
					avancements[bar],
					((double)bar) / (nbBars - 1),
					durations[bar],
					posInMelody,
					melody_asChordsIndices,
					instrPlays_nothing_rhythm_melody
				);
				// Incrémenter la position dans la mélodie
				if (bar == barMelodyBegin - 1) posInMelody = 0;
				else if (bar >= barMelodyBegin && seq[bar] != 4) posInMelody += durations[bar] / 4;
			}

			// Retourner
			return block;

		}

	// Fonctions
	private:
		array<int, 1>^ getNewBarsSeq() {

			array<int, 1>^ ans;
			
			// Créer la chaîne de Markov
			Markov^ chain = getBarsSeqMarkovInitialized();
			
			// Créer des séquences aléatoires, répéter jusqu'à trouver une séquence contenant au moins deux fois chaque type
			int cpt = 0;
			do {
				// Créer une séquence (sans vérifications)
				ans = getNewBarsSeq_noChecks(chain);
				// Incrémenter
				if (++cpt == 10000) throw gcnew Exception("Trop d'essais infructueux. Essayez un 'nbBars' plus grand, ou modifiez les probabilités pour éviter qu'un type de mesure vienne trop rarement.");
			} while (barsSeqIsInvalid(ans));

			// Retourner
			return ans;

		}
		array<int, 1>^ getNewBarsSeq_noChecks(Markov^ chain) {

			// Générer le bon nombre d'éléments
			array<int, 1>^ ans = gcnew array<int, 1>(nbBars);
			for (int i = 0; i < nbBars; i++) {
				ans[i] = chain->next(i, nbBars);
			}

			// Retourner
			return ans;

		}
		bool barsSeqIsInvalid(array<int, 1>^ barsSeq) {
		
			// Déterminer le nombre de types de mesures différents
			int nbTypes = 0;
			for each (int type in barsSeq) nbTypes = Math::Max(nbTypes, type);
			nbTypes++;

			// Compter les occurrences de chaque type
			array<int, 1>^ occurr = gcnew array<int, 1>(nbTypes);
			for each (int type in barsSeq) occurr[type]++;

			// Si un type vient moins de deux fois : la séquence est invalide
			for each (int type in barsSeq) if (occurr[type] < 2) return true;

			// Sinon, la séquence est valide
			return false;

		}
		Markov^ getBarsSeqMarkovInitialized() {
		
			// PROV : changer les probabilités

			// Créer les probabilités par étapes
			int order = 5;
			array<double, 2>^ probas1 = gcnew array<double, 2> {
				{ 1.0, 0.0, 0.0, 0.0, 0.0 },
				{ 0.0, 0.0, 0.0, 0.0, 0.0 },
				{ 0.0, 0.0, 0.0, 0.0, 0.0 },
				{ 0.0, 0.0, 0.0, 0.0, 0.0 },
				{ 0.0, 0.0, 0.0, 0.0, 0.0 }
			};
			array<double, 2>^ probas2 = gcnew array<double, 2> {
				{ 0.55, 0.45, 0.0, 0.0, 0.1 },
				{ 0.55, 0.45, 0.0, 0.0, 0.1 },
				{ 0.55, 0.45, 0.0, 0.0, 0.1 },
				{ 0.55, 0.45, 0.0, 0.0, 0.1 },
				{ 0.55, 0.45, 0.0, 0.0, 0.1 }
			};
			array<double, 2>^ probas3 = gcnew array<double, 2> {
				{ 0.3, 0.3, 0.3, 0.0, 0.1 },
				{ 0.3, 0.3, 0.3, 0.0, 0.1 },
				{ 0.3, 0.3, 0.3, 0.0, 0.1 },
				{ 0.3, 0.3, 0.3, 0.0, 0.1 },
				{ 0.3, 0.3, 0.3, 0.0, 0.1 }
			};
			array<double, 2>^ probas4 = gcnew array<double, 2> {
				{ 0.3, 0.7, 0.0, 0.0, 0.0 },
				{ 0.7, 0.3, 0.0, 0.0, 0.0 },
				{ 0.5, 0.5, 0.0, 0.0, 0.0 },
				{ 0.5, 0.5, 0.0, 0.0, 0.0 },
				{ 0.5, 0.5, 0.0, 0.0, 0.0 }
			};
			array<double, 2>^ probas5 = gcnew array<double, 2> {
				{ 0.2, 0.2, 0.2, 0.2, 0.2 },
				{ 0.2, 0.2, 0.2, 0.2, 0.2 },
				{ 0.2, 0.2, 0.2, 0.2, 0.2 },
				{ 0.2, 0.2, 0.2, 0.2, 0.2 },
				{ 0.2, 0.2, 0.2, 0.2, 0.2 }
			};

			// Créer les probabilités comme des variables temporelles
			array<TimeVariable^, 2>^ probas = gcnew array<TimeVariable^, 2>(order, order);
			for (int i = 0; i < order; i++) {
				for (int j = 0; j < order; j++) {
					probas[i, j] = gcnew TimeVariable_LinMulti(
						gcnew array<double, 1> { 0            , 0.15         , 0.3          , 0.65         , 1             },
						gcnew array<double, 1> { probas1[i, j], probas2[i, j], probas3[i, j], probas4[i, j], probas5[i, j] }
					);
				}
			}

			// Retourner
			Markov^ ans = gcnew Markov(order, probas, true);
			ans->initialize(0);
			return ans;

		}
		array<double, 1>^ getBarsAvancements(array<int, 1>^ barsSeq) {
		
			// Déterminer le nombre de types de mesures différents
			int nbTypes = 0;
			for each (int type in barsSeq) nbTypes = Math::Max(nbTypes, type);
			nbTypes++;

			// Compter les occurrences de chaque type
			array<int, 1>^ occurr = gcnew array<int, 1>(nbTypes);
			for each (int type in barsSeq) occurr[type]++;
			
			// Initialiser les compteurs par type
			array<int, 1>^ cpt = gcnew array<int, 1>(nbTypes);
			for (int i = 0; i < nbTypes; i++) cpt[i] = 0;

			// Créer le tableau des avancements
			array<double, 1>^ ans = gcnew array<double, 1>(nbBars);
			for (int i = 0; i < nbBars; i++) {
				int thisType = barsSeq[i];
				ans[i] = ((double)cpt[thisType]) / (occurr[thisType] - 1);
				cpt[barsSeq[i]]++;
			}

			// Retourner
			return ans;

		}
		array<int, 1>^ getDurations(array<int, 1>^ seq, array<double, 1>^ avancements) {
		
			array<int, 1>^ ans = gcnew array<int, 1>(nbBars);

			for (int i = 0; i < nbBars; i++) ans[i] = getDuration_32nds(seq[i], avancements[i]);

			return ans;
		
		}
		array<int, 2>^ getInstrPlays_nothing_rhythm_melody(array<int, 1>^ barsSeq) {
			
			// Initialiser à 0 : 'nothing'
			array<int, 2>^ ans = gcnew array<int, 2>(nbInstr + 1, nbBars);
			for (int instr = 1; instr <= nbInstr; instr++) for (int bar = 0; bar < nbBars; bar++) ans[instr, bar] = 0;

			// Instants
			array<int, 1>^ times = gcnew array<int, 1> {
				(int)Math::Round(0.0 * nbBars),
				(int)Math::Round(0.1 * nbBars),
				(int)Math::Round(0.2 * nbBars),
				(int)Math::Round(0.3 * nbBars),
				(int)Math::Round(0.4 * nbBars),
				(int)Math::Round(0.5 * nbBars),
				(int)Math::Round(0.6 * nbBars),
				(int)Math::Round(0.7 * nbBars),
				(int)Math::Round(0.8 * nbBars),
				(int)Math::Round(0.9 * nbBars),
				(int)Math::Round(1.0 * nbBars)
			};
		
			// Fixer qui joue le rythme (version non aléatoire)
			{
				for (int bar = times[0]; bar < times[ 3]; bar++)   ans[p_tbn    , bar] = 1; // Trombone
				for (int bar = times[0]; bar < times[ 7]; bar++)   ans[p_grc    , bar] = 1; // Grosse caisse
				for (int bar = times[0]; bar < times[10]; bar++)   ans[p_cl     , bar] = 1; // Grosse caisse
				for (int bar = times[0]; bar < times[ 6]; bar++) { ans[p_al     , bar] = 1; // Cordes
				                                                   ans[p_vc     , bar] = 1;
				                                                   ans[p_cb     , bar] = 1; }
				for (int bar = times[5]; bar < times[10]; bar++)   ans[p_pno + 1, bar] = 1; // Piano main gauche
				for (int bar = times[7]; bar < times[10]; bar++)   ans[p_vib    , bar] = 1; // Vibraphone
			}

			// Fixer qui joue la mélodie (version non aléatoire)
			{
				for (int bar = times[0]; bar < times[10]; bar++)   ans[p_hb , bar] = 2; // Hautbois
				for (int bar = times[2]; bar < times[10]; bar++)   ans[p_tpt, bar] = 2; // Trompette
				for (int bar = times[4]; bar < times[10]; bar++)   ans[p_tbn, bar] = 2; // Trombone
				for (int bar = times[6]; bar < times[10]; bar++) { ans[p_vl , bar] = 2; // Cordes
				                                                   ans[p_al , bar] = 2;
				                                                   ans[p_vc , bar] = 2;
				                                                   ans[p_cb , bar] = 2; }
				for (int bar = times[8]; bar < times[10]; bar++)   ans[p_pno, bar] = 2; // Piano main droite
			}

			// Introduction d'aléatoire : parfois...
			for (int bar = 0; bar < nbBars; bar++) {
				// ... ajouter des toms sur la rythmique
				if (Rand::double01() < 0.075) ans[p_tom, bar] = 1;
				// ... enlever la grosse caisse ou la main gauche du piano
				if (Rand::double01() < 0.185) ans[p_grc, bar] = 0;
				if (Rand::double01() < 0.285) ans[p_pno + 1, bar] = 0;
			}

			// Introduction d'aléatoire : faire quelques échanges de colonnes adjacentes
			{
				int nbEch = nbBars; // modifiable, mais doit rester proportionnel à 'nbBars' : '(int)Math::Round(0.5 * nbBars)'
				for (int n = 0; n < nbEch; n++) {
					int bar1 = Rand::integer(0, nbBars - 1);
					int bar2 = bar1 + 1;
					for (int instr = 1; instr <= nbInstr; instr++) {
						int exchange____ = ans[instr, bar1];
						ans[instr, bar1] = ans[instr, bar2];
						ans[instr, bar2] = exchange____;
					}
				}
			}

			// Retourner
			return ans;

		}
		int getMelodyLength_eighths(int barMelodyBegin, array<int, 1>^ seq, array<int, 1>^ durations) {
		
			int ans = 0;

			// Additionner les durées (en triples croches), en sautant les mesures de break
			for (int bar = barMelodyBegin; bar < nbBars; bar++) {
				if (seq[bar] != 4) {
					ans += durations[bar];
				}
			}

			// Retourner, en croches
			return ans / 4;
		
		}

	// Mélodie
	private:
		array<array<int, 1>^, 1>^ getMelody_asChordsIndices /* chaque case est un tableau contenant un ou plusieurs indices */ (int nb) {
			
			// Décider d'un déroulement par items
			List<int>^ items_lst = gcnew List<int>;
			List<int>^ eighthsAfter_lst = gcnew List<int>;
			int beg = createMelodicItemsList_returnAvancementBegin(items_lst, eighthsAfter_lst, nb);
			array<int, 1>^ items = items_lst->ToArray();
			array<int, 1>^ eighthsAfter = eighthsAfter_lst->ToArray();
			int nbItems = items->Length;

			// Placer chaque item
			array<array<int, 1>^, 1>^ ans = gcnew array<array<int, 1>^, 1>(nb);
			for (int i = 0; i < nb; i++) ans[i] = gcnew array<int, 1>(0);
			int pos = 0;
			int cptForAvancement = beg;
			for (int i = 0; i < nbItems; i++) {
				int melodicItemType = items[i];
				int eighthsAfterThisItem = eighthsAfter[i];
				double avancement = ((double)(cptForAvancement - beg)) / (nb - 1 - beg);
				addMelodicItem(ans, pos, melodicItemType, avancement);
				int eighthsAdded = melodicItemLength(melodicItemType) + eighthsAfterThisItem;
				pos += eighthsAdded;
				cptForAvancement += eighthsAdded;
			}

			// Ajouter un nombre croissant de notes dans chaque case
			addNotesInSomeCells(ans);

			// Retourner
			return ans;

		}
		int createMelodicItemsList_returnAvancementBegin(List<int>^ items_out, List<int>^ eighthsAfter_out, int nb) {
		
			// Ajouter les éléments précomposés
			items_out->Add(0); eighthsAfter_out->Add(1);
			items_out->Add(1); eighthsAfter_out->Add(1);
			items_out->Add(2); eighthsAfter_out->Add(2);
			items_out->Add(0); eighthsAfter_out->Add(1);
			items_out->Add(3); eighthsAfter_out->Add(3);
			items_out->Add(4); eighthsAfter_out->Add(1);
			items_out->Add(0); eighthsAfter_out->Add(1);
			
			// Déterminer le début après ces éléments précomposés
			int beg = 0; for (int item = 0; item < items_out->Count; item++) beg += melodicItemLength(items_out[item]) + eighthsAfter_out[item];

			// Ajouter de nouveaux éléments et du temps entre les éléments
			int pos = beg;
			const double junction = 0.75;
			while (pos < nb) {
				// Avancement
				double avancement = ((double)(nb - 1 - pos)) / (nb - 1 - beg);
				// Ajouter un item
				char whichSection; {
					if (avancement <= junction) {
						double av = avancement / junction;
						double prop_A = 0.5 + 0.5 * Math::Cos(av * Math::PI);
						double prop_B = 1 - prop_A;
						whichSection = (Rand::double01() < prop_A) ? 'A' : 'B';
					} else {
						double av = (avancement - junction) / (1 - junction);
						double prop_B = 1;
						const double C_level = 1.5;
						double prop_C = C_level * (0.5 - 0.5 * Math::Cos(av * Math::PI));
						double div = prop_B + prop_C;
						prop_B /= div;
						prop_C /= div;
						whichSection = (Rand::double01() < prop_B) ? 'B' : 'C';
					}
				}
				int min, max; {
					if (whichSection == 'A') {
						min = 0;
						max = 5;
					} else if (whichSection == 'B') {
						min = 6;
						max = 10;
					} else if (whichSection == 'C') {
						min = 11;
						max = 12;
					} else {
						throw gcnew Exception("Item mélodique inconnu.");
					}
				}
				int whichItem = Rand::integer(min, max + 1);
				items_out->Add(whichItem);
				// Ajouter du temps
				double av = avancement / 0.75; if (av > 1) av = 1;
				double time_min = 1 - av;
				double time_max = 3.49 * (1 - av);
				double time_dbl = time_min + (time_max - time_min) * Rand::double01();
				int time = (int)Math::Round(time_dbl);
				eighthsAfter_out->Add(time);
				// Incrémenter
				pos += melodicItemLength(whichItem) + time;
			}
			{
				// Mettre à zéro le dernier temps additionnel
				int lastIndex = eighthsAfter_out->Count - 1;
				int remove = eighthsAfter_out[lastIndex];
				eighthsAfter_out[lastIndex] = 0;
				pos -= remove;
				if (pos >= nb) {
					// Enlever le dernier item, enlever le dernier temps additionnel, mettre à zéro l'avant-dernier temps additionnel
					remove = melodicItemLength(items_out[lastIndex]) + eighthsAfter_out[lastIndex - 1];
					items_out->RemoveAt(lastIndex);
					eighthsAfter_out->RemoveAt(lastIndex);
					eighthsAfter_out[lastIndex - 1] = 0;
					pos -= remove;
				}
			}
			{
				// Ajouter des croches pour tomber juste à la fin
				int add = nb - pos;
				for (int n = 0; n < add; n++) {
					int index = Rand::integer(0, eighthsAfter_out->Count);
					eighthsAfter_out[index]++;
				}
			}

			// Retourner l'emplacement du début
			return beg;

		}
		int melodicItemLength(int melodicItemType) {
		
			switch (melodicItemType) {
				// Section A
				case 0 : return 6;
				case 1 : return 6;
				case 2 : return 4;
				case 3 : return 4;
				case 4 : return 2;
				case 5 : return 2;
				// Section B
				case 6 : return 4;
				case 7 : return 4;
				case 8 : return 3;
				case 9 : return 3;
				case 10: return 3;
				// Section C
				case 11: return 3;
				case 12: return 3;
				// Erreur
				default: throw gcnew Exception("Item mélodique inconnu.");
			}
		
		}
		void addMelodicItem(array<array<int, 1>^, 1>^ arr, int pos, int melodicItemType, double avancement) {
		
			switch (melodicItemType) {
				// Rien à ajouter
				case -1: break;
				// Groupe A
				case  0: arr[pos] = newArray(3); arr[pos + 2] = newArray(6); arr[pos + 4] = newArray(7); break;
				case  1: arr[pos] = newArray(4); arr[pos + 2] = newArray(6); arr[pos + 4] = newArray(8); break;
				case  2: arr[pos] = newArray(3); arr[pos + 2] = newArray(6); break;
				case  3: arr[pos] = newArray(4); arr[pos + 2] = newArray(6); break;
				case  4: arr[pos] = newArray(4); break;
				case  5: arr[pos] = newArray(3); break;
				// Groupe B
				case  6: arr[pos] = newArray(3); arr[pos + 1] = newArray(6); arr[pos + 2] = newArray(7); break;
				case  7: arr[pos] = newArray(4); arr[pos + 1] = newArray(6); arr[pos + 2] = newArray(8); break;
				case  8: arr[pos] = newArray(3); arr[pos + 1] = newArray(6); break;
				case  9: arr[pos] = newArray(6); arr[pos + 1] = newArray(7); break;
				case 10: arr[pos] = newArray(6); arr[pos + 1] = newArray(8); break;
				// Groupe C
				case 11: arr[pos] = newArray(Rand::integer(4, 9)); arr[pos + 1] = newArray(9); break;
				case 12: {
					int a, b; do { a = Rand::integer(3, 8); b = Rand::integer(4, 9); } while (a <= b || a - b > 4);
					arr[pos] = newArray(a); arr[pos + 1] = newArray(b); arr[pos + 2] = newArray(9); break;
				}
				// Erreur
				default: throw gcnew Exception("Item mélodique inconnu.");
			}
		
		}
		void addNotesInSomeCells(array<array<int, 1>^, 1>^ arr) {
			
			// Evolution de la probabilité d'ajouter des notes
			TimeVariable^ proba; {
				double mult = 0.7;
				proba = gcnew TimeVariable_LinMulti(
					mult * 0.0,
					mult * 0.3,
					mult * 0.7,
					mult * 0.4,
					mult * 0.2,
					mult * 1.0
				);
			}

			// Parcourir le temps et ajouter des notes
			int timeMin = 1;
			int timeMax = nbBars - 2;
			for (int i = timeMin; i <= timeMax; i++) {
				double avancement = ((double)(i - timeMin)) / (timeMax - timeMin);
				// La position dans le groupe est-elle adaptée pour ajouter des notes ?
				bool ok_add; {
					int last = arr[i - 1]->Length;
					int curr = arr[i    ]->Length;
					int next = arr[i + 1]->Length;
					if      (curr == 0) ok_add = false;
					else if (last == 0) ok_add = Rand::double01() < 0.85;
					else                ok_add = (next > 0) ? Rand::double01() < 0.08 : Rand::double01() < 0.5;
				}
				// Appliquer la probabilité
				ok_add = ok_add && proba->at(avancement);
				// Si on veut ajouter des notes :
				if (ok_add) {
					// Quelle est la note actuelle ?
					int current = arr[i][0];
					// Combien veut-on en ajouter au maximum ?
					int addMax = (int)Math::Round(3 * avancement);
					addMax = Math::Min(addMax, current);
					// Combien veut-on en ajouter ?
					int add = Rand::integer(0, addMax + 1);
					// Ajouter les notes
					arr[i] = gcnew array<int, 1>(add + 1);
					arr[i][add] = current;
					for (int j = add - 1; j >= 0; j--) arr[i][j] = arr[i][j + 1] - 1;
				}
			}
		
		}
		Pitch^ associateMelodyIndexToPitch(int index) {
			
			switch (index) {
				case -1: return nullptr;
				case  0: return gcnew Pitch(58);
				case  1: return gcnew Pitch(60);
				case  2: return gcnew Pitch(62);
				case  3: return gcnew Pitch(64);
				case  4: return gcnew Pitch(66);
				case  5: return gcnew Pitch(68.5);
				case  6: return gcnew Pitch(71);
				case  7: return gcnew Pitch(73);
				case  8: return gcnew Pitch(75);
				case  9: return gcnew Pitch(77);
				default: throw gcnew Exception("Index mélodique inconnu.");
			}

		}

	// Contenu musical
	private:
		int getDuration_32nds(int barType, double avancement) {
			
			switch (barType) {
				case 0: return 4 * 4;
				case 1: return 4 * 4;
				case 2: return 4 * 3;
				case 3: return 4 * 3;
				case 4: return getDuration_32nds_caseTomsBreak(avancement);
				default: throw gcnew Exception("Type de mesure inconnu.");
			}
		
		}
		int getDuration_32nds_caseTomsBreak(double avancement) {
			
			// Définir la durée en croches entières
			int maxDur_eighths = 5;
			int dur_eighths = Rand::integer(1, maxDur_eighths + 1);
			int ans = 4 * dur_eighths;
						
			// Définir la probabilité que la durée ait un nombre de croches non entier
			const double proba_strange_max = 0.7;
			double proba_strange = proba_strange_max * (0.5 - 0.5 * Math::Cos(avancement * Math::PI));
			bool strange = (Rand::double01() < proba_strange);
			
			// Ajuster si la durée est étrange
			if (strange) ans += (Rand::double01() > 0.5) ? +1 : -1;
			
			// Retourner
			return ans;

		}
		void addNewBarToBlock(Block^ block, int bar, int barType, double avancementByType, double globalAvancement, int barDuration_32nds, int posInMelody, array<array<int, 1>^, 1>^ melody_asChordsIndices, array<int, 2>^ instrPlays_nothing_rhythm_melody) {
		
			// Ajouter la mesure vide
			Signature^ signature = gcnew Signature(barDuration_32nds * 32, nullptr);
			block->addBar(signature);

			// Déterminer qui joue le rythme et qui joue la mélodie sur cette mesure
			array<bool, 1> ^partsRhythm, ^partsMelody;
			{
				partsRhythm = gcnew array<bool, 1>(nbInstr + 1);
				partsMelody = gcnew array<bool, 1>(nbInstr + 1);
				for (int part = 1; part <= nbInstr; part++) {
					int role = instrPlays_nothing_rhythm_melody[part, bar];
					if      (role == 1) partsRhythm[part] = true;
					else if (role == 2) partsMelody[part] = true;
				}
			}

			// Ajouter les parties jouant le rythme et les parties jouant la mélodie
			createRhythmicParts(block, bar, barType     , partsRhythm, avancementByType, globalAvancement, barDuration_32nds);
			createMelodicParts (block, bar, barType != 4, partsMelody,                                     barDuration_32nds, posInMelody, melody_asChordsIndices);
		
		}
		void createRhythmicParts(Block^ block, int bar, int barType, array<bool, 1>^ parts, double avancementByType, double globalAvancement, int barDuration_32nds) {
			
			switch (barType) {
				case  0: createRhythmicParts_barType0(block, bar, parts, avancementByType, globalAvancement); break;
				case  1: createRhythmicParts_barType1(block, bar, parts, avancementByType, globalAvancement); break;
				case  2: createRhythmicParts_barType2(block, bar, parts, avancementByType, globalAvancement); break;
				case  3: createRhythmicParts_barType3(block, bar, parts, avancementByType, globalAvancement); break;
				case  4: createRhythmicParts_barType4(block, bar, parts, avancementByType, globalAvancement, barDuration_32nds); break;
				default: throw gcnew Exception("Type de mesure inconnu.");
			}

		}
		void createMelodicParts(Block^ block, int bar, bool barTypeHasMelody, array<bool, 1>^ parts, int barDuration_32nds, int posInMelody, array<array<int, 1>^, 1>^ melody_asChordsIndices) {
			
			// Arrêtersi la mélodie n'a pas encore commencé, ou si cette mesure ne doit pas avoir de mélodie
			if (posInMelody < 0 || !barTypeHasMelody) return;

			// Récupérer la durée, en croches
			int nb = barDuration_32nds / 4;

			// Créer les mélodies successives associée aux indices, pour chaque croche
			array<array<Pitch^, 1>^, 1>^ melodies = gcnew array<array<Pitch^, 1>^, 1>(nb); {
				for (int i = 0; i < nb; i++) {
					array<int, 1>^ mel_asChordsIndices = melody_asChordsIndices[posInMelody + i];
					int melLen = mel_asChordsIndices->Length;
					melodies[i] = gcnew array<Pitch^, 1>(melLen);
					for (int j = 0; j < melLen; j++) {
						melodies[i][j] = associateMelodyIndexToPitch(mel_asChordsIndices[j]);
					}
				}
			}

			// Ajouter chaque croche à chaque instrument
			array<List<Element^>^, 1>^ elements = gcnew array<List<Element^>^, 1>(nbInstr + 1);
			for (int i = 1; i <= nbInstr; i++) if (parts[i]) elements[i] = gcnew List<Element^>;
			for (int eighth = 0; eighth < nb; eighth++) {
				if (parts[p_hb ]) appendInMelodicPart_eighth(elements[p_hb ], melodies[eighth], newArray(0));
				if (parts[p_tpt]) appendInMelodicPart_eighth(elements[p_tpt], melodies[eighth], newArray(-8));
				if (parts[p_tbn]) appendInMelodicPart_eighth(elements[p_tbn], melodies[eighth], newArray(-30));
				if (parts[p_vl ]) appendInMelodicPart_eighth(elements[p_vl ], melodies[eighth], newArray(-11));
				if (parts[p_al ]) appendInMelodicPart_eighth(elements[p_al ], melodies[eighth], newArray(-19));
				if (parts[p_vc ]) appendInMelodicPart_eighth(elements[p_vc ], melodies[eighth], newArray(-25));
				if (parts[p_cb ]) appendInMelodicPart_eighth(elements[p_cb ], melodies[eighth], newArray(-28));
				if (parts[p_pno]) appendInMelodicPart_eighth(elements[p_pno], melodies[eighth], newArray(-8, 2));
			}

			// Ajouter au bloc
			for (int instr = 1; instr <= nbInstr; instr++) {
				if (parts[instr]) {
					block[instr, bar + 1] = gcnew Group(elements[instr]);
				}
			}

		}
		void appendInMelodicPart_eighth(List<Element^>^ elements_out, array<Pitch^, 1>^ melody, array<int, 1>^ transpositions) {
			
			// Nombre de notes dans cette croche
			int nb = melody->Length;
		
			// Rythme : 0  = demi-soupir                
			//          1  = croche						
			//          21 = deux doubles				
			//          22 = croche avec 1 appoggiature	
			//          31 = triolets de doubles		
			//          32 = croche avec 2 appoggiatures
			//          41 = triples					
			//          42 = croche avec 3 appoggiatures
			int rhythmType; {
				if      (nb == 0) rhythmType = 0;
				else if (nb == 1) rhythmType = 1;
				else if (nb == 2) rhythmType = (Rand::double01() < 0.5) ? 21 : 22;
				else if (nb == 3) rhythmType = (Rand::double01() < 0.5) ? 31 : 32;
				else if (nb == 4) rhythmType = (Rand::double01() < 0.5) ? 41 : 42;
				else              throw gcnew Exception("Type rythmique inconnu dans la mélodie.");
			}
		
			// Créer le tableau de pitches
			array<array<Pitch^, 1>^, 1>^ pitches; {
				pitches = gcnew array<array<Pitch^, 1>^, 1>(nb);
				int nbPitchesInEachChord = transpositions->Length;
				for (int i = 0; i < nb; i++) {
					pitches[i] = gcnew array<Pitch^, 1>(nbPitchesInEachChord);
					for (int j = 0; j < nbPitchesInEachChord; j++) {
						pitches[i][j] = melody[i] + (double)(transpositions[j]);
					}
				}
			}
			
			// Ajouter les éléments
			if (rhythmType == 0) {
				elements_out->Add(qElement("e"));
			} else if (rhythmType == 1) {
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitches[0]));
			} else if (rhythmType == 21) {
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitches[0]));
				elements_out->Add(gcnew Note(RhythmFigure::_16th, pitches[1]));
			} else if (rhythmType == 22) {
				elements_out->Add(Note::graceNote(RhythmFigure::_16th, pitches[0]));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitches[1]));
			} else if (rhythmType == 31) {
				elements_out->Add(gcnew Tuplet(
					gcnew TupletRatio(RhythmFigure::_16th, 3, 2),
					gcnew array<NoteOrRest^, 1> {
						gcnew Note(RhythmFigure::_16th, pitches[0]),
						gcnew Note(RhythmFigure::_16th, pitches[1]),
						gcnew Note(RhythmFigure::_16th, pitches[2])
					}
				));
			} else if (rhythmType == 32) {
				elements_out->Add(Note::graceNote(RhythmFigure::_16th, pitches[0]));
				elements_out->Add(Note::graceNote(RhythmFigure::_16th, pitches[1]));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitches[2]));
			} else if (rhythmType == 41) {
				elements_out->Add(gcnew Note(RhythmFigure::_32nd, pitches[0]));
				elements_out->Add(gcnew Note(RhythmFigure::_32nd, pitches[1]));
				elements_out->Add(gcnew Note(RhythmFigure::_32nd, pitches[2]));
				elements_out->Add(gcnew Note(RhythmFigure::_32nd, pitches[3]));
			} else if (rhythmType == 42) {
				elements_out->Add(Note::graceNote(RhythmFigure::_16th, pitches[0]));
				elements_out->Add(Note::graceNote(RhythmFigure::_16th, pitches[1]));
				elements_out->Add(Note::graceNote(RhythmFigure::_16th, pitches[2]));
				elements_out->Add(gcnew Note(RhythmFigure::_eighth, pitches[3]));
			} else {
				throw gcnew Exception("Type rythmique inconnu.");
			}

		}

	// Contenu des parties rythmiques des différents types de mesures
	private:
		void createRhythmicParts_barType0(Block^ block, int bar, array<bool, 1>^ parts, double avancementByType, double globalAvancement) {
			
			createRhythmicParts(
				block, bar,
				/* on  */ qGroup("e|* e e|* e"),
				/* off */ qGroup("e e|* e e|*"),
				/* mix */ qGroup("e|* e|* e e|*"),
				parts, avancementByType, globalAvancement
			);
		
		}
		void createRhythmicParts_barType1(Block^ block, int bar, array<bool, 1>^ parts, double avancementByType, double globalAvancement) {
			
			createRhythmicParts(
				block, bar,
				/* on  */ qGroup("e|* e e|* e"),
				/* off */ qGroup("e [3:2 e|* e|* e|*] e|*"),
				/* mix */ qGroup("e|* [3:2 e|* e|* e|*] e|*"),
				parts, avancementByType, globalAvancement
			);
					
		}
		void createRhythmicParts_barType2(Block^ block, int bar, array<bool, 1>^ parts, double avancementByType, double globalAvancement) {
			
			createRhythmicParts(
				block, bar,
				/* on  */ qGroup("e|* e e"),
				/* off */ qGroup("e [3:2 e|* e|* e|*]"),
				/* mix */ qGroup("e|* [3:2 e|* e|* e|*]"),
				parts, avancementByType, globalAvancement
			);
					
		}
		void createRhythmicParts_barType3(Block^ block, int bar, array<bool, 1>^ parts, double avancementByType, double globalAvancement) {
			
			createRhythmicParts(
				block, bar,
				/* on  */ qGroup("e|* e e"),
				/* off */ qGroup("e e|* e|*"),
				/* mix */ qGroup("e|* e|* e|*"),
				parts, avancementByType, globalAvancement
			);
					
		}
		void createRhythmicParts_barType4(Block^ block, int bar, array<bool, 1>^ parts, double avancementByType, double globalAvancement, int barDuration_32nds) {
			
			// Durée
			int nbEighths = (int)Math::Round(((double)barDuration_32nds) / 4);
			int add32nds = barDuration_32nds - 4 * nbEighths;

			// Décomposition rythmique : 2 = doubles, 3 = triolets de doubles, 4 = triples
			int rhythmicDecomposition; {
				if (add32nds != 0) {
					rhythmicDecomposition = 4;
				} else {
					double proba_2 = 0.25 + (0.5 + 0.5 * Math::Cos(avancementByType * Math::PI));
					double proba_3 = 0.6;
					double proba_4 = 0.25 + (0.5 - 0.5 * Math::Cos(avancementByType * Math::PI));
					double sum = proba_2 + proba_3 + proba_4;
					proba_2 /= sum;
					proba_3 /= sum;
					proba_4 /= sum;
					double rand = Rand::double01();
					if      (rand < proba_2          ) rhythmicDecomposition = 2;
					else if (rand < proba_2 + proba_3) rhythmicDecomposition = 3;
					else                               rhythmicDecomposition = 4;
				}
			}
			
			// Succession des notes de chaque croche : 4 = tom aigu, 1 = tom grave -- nombre de nbMutations à introduire
			array<int, 1>^ eighths_pitches;
			int nbMutations;
			{
				switch (nbEighths) {
					case 1: {
						bool high = Rand::double01() < 0.5;
						if (high) eighths_pitches = gcnew array<int, 1> { 4 };
						else      eighths_pitches = gcnew array<int, 1> { 3 };
						nbMutations = high ? 0 : (Rand::double01() < 0.8 ? 1 : 0);
						break;
					}
					case 2: {
						eighths_pitches = gcnew array<int, 1> { 4, 3 };
						nbMutations = 0;
						break;
					}
					case 3: {
						bool reverse = Rand::double01() < 0.5 * (avancementByType - 0.5) + 0.5;
						if (reverse) eighths_pitches = gcnew array<int, 1> { 4, 2, 3 };
						else         eighths_pitches = gcnew array<int, 1> { 4, 3, 2 };
						nbMutations = reverse ? 0 : (Rand::double01() < 0.5 ? 1 : 0);
						break;
					}
					case 4: {
						bool reverse = Rand::double01() < 0.5 * (avancementByType - 0.5) + 0.5;
						if (reverse) eighths_pitches = gcnew array<int, 1> { 4, 3, 1, 2 };
						else         eighths_pitches = gcnew array<int, 1> { 4, 3, 3, 2 };
						nbMutations = (Rand::double01() < 0.5 * (avancementByType - 0.5) + 0.5) ? 1 : 0;
						break;
					}
					case 5: {
						int nbTypes = 3;
						int type = Rand::integer(1, nbTypes + 1);
						if      (type == 1) eighths_pitches = gcnew array<int, 1> { 4, 3, 3, 1, 2 };
						else if (type == 2) eighths_pitches = gcnew array<int, 1> { 4, 3, 3, 2, 2 };
						else if (type == 3) eighths_pitches = gcnew array<int, 1> { 3, 4, 4, 2, 3 };
						nbMutations = (Rand::double01() < 0.5 * (avancementByType - 0.5) + 0.5) ? 1 : 0;
						break;
					}
					default: throw gcnew Exception("Si vous prévoyez des breaks de cette longueur, rajoutez le 'case' correspondant.");
				}
			}

			// Succession de notes : pour chaque croche, choisir : - décomposé comment ? - durée modifiée ? - mutation introduite ?
			array<int , 1>^ eighths_decompositions;
			array<int , 1>^ eighths_durationsModified;
			array<bool, 1>^ eighths_mutation;
			{
				// Initialiser aux valeurs les plus probables
				{
					eighths_decompositions    = gcnew array<int , 1>(nbEighths);
					eighths_durationsModified = gcnew array<int , 1>(nbEighths);
					eighths_mutation          = gcnew array<bool, 1>(nbEighths);
					for (int i = 0; i < nbEighths; i++) {
						eighths_decompositions   [i] = rhythmicDecomposition;
						eighths_durationsModified[i] = 0;
						eighths_mutation         [i] = false;
					}
				}
				// Introduire une éventuelle modification de décomposition
				bool modifDecomposition = (nbEighths >= 2) && (Rand::double01() < avancementByType);
				if (modifDecomposition) {
					// Position de la modification
					int posModif = Rand::integer(1, nbEighths);
					// Modifier
					int currentDecomposition = eighths_decompositions[posModif];
					int newValue;
					do newValue = Rand::integer(2, 5); while (newValue == currentDecomposition);
					eighths_decompositions[posModif] = newValue;
				}
				// Introduire la modification de durée sur une croche aléatoire
				if (add32nds != 0) {
					// Position de la modification
					int posModif = Rand::integer(0, nbEighths);
					// Modifier
					eighths_durationsModified[posModif] = add32nds;
				}
				// Introduire les nbMutations
				for (int n = 0; n < nbMutations; n++) {
					// Position de la mutation
					int posModif;
					do posModif = (int)Math::Floor(Math::Sqrt(Rand::double01()) * (nbEighths - 0.0001)); while (eighths_mutation[posModif]);
					// Muter
					eighths_mutation[posModif] = true;
				}
			}

			// Réaliser en utilisant 'eighths_decompositions', 'eighths_pitches', 'eighths_mutation', 'eighths_durationsModified'
			List<Element^>^ elements = gcnew List<Element^>; {
				for (int i = 0; i < nbEighths; i++) {
					// Décomposition
					int decomposition = eighths_decompositions[i];
					// Nombre de notes
					int nb = decomposition + eighths_durationsModified[i];
					// Hauteurs
					array<int, 1>^ pitches = gcnew array<int, 1>(nb); {
						int globalPitch = eighths_pitches[i];
						for (int j = 0; j < nb; j++) pitches[j] = globalPitch;
						if (eighths_mutation[i]) {
							int posMutation = Rand::integer(0, nb);
							int newPitch;
							do newPitch = Rand::integer(1, 5); while (newPitch == globalPitch);
							pitches[posMutation] = newPitch;
						}
					}
					// Ajouter
					if (decomposition == 2) {
						for (int j = 0; j < nb; j++) elements->Add(qElement("16th|" + pitches[j] + "/4"));
					} else if (decomposition == 3) {
						for (int j = 0; j < nb; j += 3) elements->Add(qElement("[3:2 " + ("16th|" + pitches[j] + "/4") + " " + ("16th|" + pitches[j + 1] + "/4") + " " + ("16th|" + pitches[j + 2] + "/4") + "]"));
					} else if (decomposition == 4) {
						for (int j = 0; j < nb; j++) elements->Add(qElement("32nd|" + pitches[j] + "/4"));
					} else {
						throw gcnew Exception("Décomposition inconnue.");
					}
				}
			}

			// Enregistrer dans le block, partie 'toms'
			block[p_tom, bar + 1] = gcnew Group(elements);
			
		}
	private:
		void createRhythmicParts(Block^ block, int bar, Group^ rhythm_on, Group^ rhythm_off, Group^ rhythm_mix,  array<bool, 1>^ parts, double avancementByType, double globalAvancement) {
		
			if (parts[p_grc    ]) createRhythmicPart_grcTom(block, bar, avancementByType, globalAvancement, rhythm_on );
			if (parts[p_tbn    ]) createRhythmicPart_tbn   (block, bar, avancementByType, globalAvancement, rhythm_on );
			if (parts[p_cl     ]) createRhythmicPart_cl    (block, bar, avancementByType, globalAvancement, rhythm_mix);
			if (parts[p_al     ]) createRhythmicPart_al    (block, bar, avancementByType, globalAvancement, rhythm_off);
			if (parts[p_vc     ]) createRhythmicPart_vc    (block, bar, avancementByType, globalAvancement, rhythm_mix);
			if (parts[p_cb     ]) createRhythmicPart_cb    (block, bar, avancementByType, globalAvancement, rhythm_on );
			if (parts[p_pno + 1]) createRhythmicPart_pnoMg (block, bar, avancementByType, globalAvancement, rhythm_off);
		
		}
	private:
		void createRhythmicPart_grcTom(Block^ block, int bar, double avancementByType, double globalAvancement, Group^ rhythm) {
		
			// Est-on dans le cas où il y a deux temps à jouer en 2/4 ?
			bool thisCase = rhythm->duration_1024th == 128
				         && rhythm->nbElements == 4
						 && rhythm[0]->isNote() && rhythm[1]->isRest() && rhythm[2]->isNote() && rhythm[3]->isRest();
			
			// Veut-on déléguer le second temps au tom ?
			bool secondIsTom = thisCase && (Rand::double01() < 0.5 + 0.25 * (globalAvancement - 0.5));

			// Ecrire
			if (secondIsTom) {
				// Grosse caisse
				block[p_grc, bar + 1] = qGroup("e|1/1 e e e");
				// Toms
				int pitch = 1 + (int)Math::Floor(avancementByType * 3.9999);
				if (Rand::double01() < 0.4) pitch += 2 * (Rand::integer(0, 2)) - 1;
				if (pitch < 1) pitch = 1; else if (pitch > 4) pitch = 4;
				block[p_tom, bar + 1] = qGroup("e e e|" + pitch + "/4 e");
			} else {
				// Grosse caisse
				List<Element^>^ elements = gcnew List<Element^>;
				for each (Element^ element in rhythm->elements) {
					NoteOrRest^ element_noteOrRest = (NoteOrRest^)element;
					Rhythm^ rhythm = element_noteOrRest->rhythm;
					if (element_noteOrRest->isRest()) elements->Add(gcnew Rest(rhythm));
					else                              elements->Add(gcnew Note(rhythm, gcnew Pitch(1, 1)));
				}
				block[p_grc, bar + 1] = gcnew Group(elements);
			}
		
		}
		void createRhythmicPart_tbn(Block^ block, int bar, double avancementByType, double globalAvancement, Group^ rhythm) {
			
			block[p_tbn, bar + 1] = gcnew Group(rhythm, qPitch("48"));
		
		}
		void createRhythmicPart_cl(Block^ block, int bar, double avancementByType, double globalAvancement, Group^ rhythm) {
		
			block[p_cl, bar + 1] = gcnew Group(rhythm, qPitch("59"));
		
		}
		void createRhythmicPart_al(Block^ block, int bar, double avancementByType, double globalAvancement, Group^ rhythm) {
			
			array<Pitch^, 1>^ chord = newArray(qPitch("58"), qPitch("65"));
			array<array<Pitch^, 1>^, 1>^ repeatedChord = newArrayRepeat(chord, rhythm->countNotes());

			block[p_al, bar + 1] = gcnew Group(rhythm, repeatedChord);
		
		}
		void createRhythmicPart_vc(Block^ block, int bar, double avancementByType, double globalAvancement, Group^ rhythm) {
			
			array<Pitch^, 1>^ chord = newArray(qPitch("52"), qPitch("57"));
			array<array<Pitch^, 1>^, 1>^ repeatedChord = newArrayRepeat(chord, rhythm->countNotes());
		
			block[p_vc, bar + 1] = gcnew Group(rhythm, repeatedChord);
		
		}
		void createRhythmicPart_cb(Block^ block, int bar, double avancementByType, double globalAvancement, Group^ rhythm) {
			
			array<Pitch^, 1>^ chord = newArray(qPitch("42"), qPitch("48"));
			array<array<Pitch^, 1>^, 1>^ repeatedChord = newArrayRepeat(chord, rhythm->countNotes());
		
			block[p_cb, bar + 1] = gcnew Group(rhythm, repeatedChord);
		
		}
		void createRhythmicPart_pnoMg(Block^ block, int bar, double avancementByType, double globalAvancement, Group^ rhythm) {
			
			array<Pitch^, 1>^ chord = newArray(qPitch("57"), qPitch("58"), qPitch("60"), qPitch("61"));
			array<array<Pitch^, 1>^, 1>^ repeatedChord = newArrayRepeat(chord, rhythm->countNotes());
		
			block[p_pno + 1, bar + 1] = gcnew Group(rhythm, repeatedChord);
		
		}

};

// Fréquence des notes de la gamme
public ref class G_FreqsNotes : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Fréquence des notes de la gamme";

		}
		virtual String^ description() override {
			
			return "Partition avec la fréquence de toutes les notes.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			int midiMin = 12;
			int midiMax = 108;
			int nbNotes = midiMax - midiMin + 1;
			
			// Créer le groupe
			array<Element^, 1>^ elements = gcnew array<Element^, 1>(nbNotes);
			for (int i = 0; i < nbNotes; i++) {
				int midi = midiMin + i;
				elements[i] = gcnew Note(
					RhythmFigure::_quarter,
					gcnew Pitch(midi),
					gcnew array<Words::Words^, 1> { gcnew Words::Normal(midiToFreqStr(midi)) }
				);
			}
			Group^ group = gcnew Group(elements);

			// Créer le bloc
			Block^ block = gcnew Block(1);
			block->addBars(1, nbNotes + "/4");
			block[1, 1] = group;

			// Retourner
			return block;

		}

	private:
		static String^ midiToFreqStr(int midi) {
			
			double val = 440. * Math::Pow(2., (midi - 69) / 12.);

			val = Math::Round(val, 2);

			return ("" + val)->Replace(".", ",");

		}

};

// Toutes les séries répondant à certains critères intervalliques
public ref class G_SerialWithInterv : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Séries avec intervalles fixés";

		}
		virtual String^ description() override {
			
			return "Toutes les séries répondant à certains critères intervalliques.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			// Récupérer les options
			List<int>^ interv;
			char loopMode;
			bool removeRotations;
			int maxAmbitus;
			{
				// Montrer la boîte de dialogue
				ScoreGen::OF_SerialWithInterv^ dialog = gcnew ScoreGen::OF_SerialWithInterv(name(), description());
				dialog->ShowDialog();
				// Récupérer les valeurs des options
				array<Object^, 1>^ options = dialog->getOptions();
				if (options == nullptr) return nullptr;
				// Convertir
				interv = (List<int>^)(options[0]);
				loopMode = (char)(options[1]);
				removeRotations = (bool)(options[2]);
				maxAmbitus = (int)(options[3]);
			}

			// Faire la liste des séries qui conviennent
			List<List<int>^>^ series = getSeries(interv, loopMode, maxAmbitus, removeRotations);


				return nullptr;


			//// Créer le bloc, et lui ajouter le nombre de parties demandé sur une grande mesure
			//Block^ block = gcnew Block;
			//block->addParts(nbParts);
			//block->addBars(1, gcnew Signature(nbNotes, 4));
			//
			//// Ajouter les notes
			//for (int part = 1; part <= nbParts; part++) {
			//	List<Element^>^ elements = gcnew List<Element^>;
			//	for (int note = 0; note < nbNotes; note++) {
			//		Pitch^ pitch = gcnew Pitch(66 + nbParts + (3 + note + 2 * part) % 13 - 4 * part);
			//		elements->Add(gcnew Note(RhythmFigure::_quarter, pitch));
			//	}
			//	block[part, 1] = gcnew Group(elements);
			//}
			//
			//// Retourner
			//return block;

		}

	// Fonctions
	private:
		List<List<int>^>^ getSeries(List<int>^ interv, char loopMode, int maxAmbitus, bool removeRotations) {
			
			// Créer la liste des solutions
			List<List<int>^>^ answers = gcnew List<List<int>^>;

			// Créer le début commun à toutes les séries (un do)
			int firstNote = 60;
			List<int>^ seria = gcnew List<int>(gcnew array<int, 1>{firstNote});
			array<bool, 1>^ hitNotes = gcnew array<bool, 1>(12);
			for (int i = 0; i < 12; i++) {
				hitNotes[i] = (firstNote % 12 == i);
			}

			// Ajouter toutes les séries possibles
			addSeries_rec(answers, seria, firstNote, hitNotes, 1, interv);

			// Garder seulement les séries correspondant aux options
			answers = keep_withLoopModeAndInCorrectAmbitus(answers, loopMode, interv, maxAmbitus);
			if (removeRotations) answers = keep_removeRotations(answers);

			// Retourner
			return answers;

		}
		void addSeries_rec(List<List<int>^>^ answers, List<int>^ seria, int lastNote, array<bool, 1>^ hitNotes, int currentNbNotes, List<int>^ interv) {
		
			// Cas de base : il y a déjà 12 notes dans la série
			if (currentNbNotes == 12) {
				answers->Add(seria);
				return;
			}
			
			// Créer toutes les séries permises en ajoutant une note (avec intervalle permis, et ne répétant pas de note présente)
			for each (int interval in interv) {
				int newNote = lastNote + interval;
				int newNoteIn12 = (newNote + 1200) % 12;
				if (!hitNotes[newNoteIn12]) {
					addSeries_rec(answers, newSeriaWithNewNote(seria, newNote), newNote, newHitNotesWithNewNote(hitNotes, newNoteIn12), currentNbNotes + 1, interv);
				}
			}
		
		}
		List<int>^ newSeriaWithNewNote(List<int>^ seria, int newNote) {
		
			List<int>^ ans = gcnew List<int>;

			for each (int note in seria) ans->Add(note);

			ans->Add(newNote);

			return ans;
		
		}
		array<bool, 1>^ newHitNotesWithNewNote(array<bool, 1>^ hitNotes, int newNoteIn12) {
		
			array<bool, 1>^ ans = gcnew array<bool, 1>(12);

			for (int i = 0; i < 12; i++) ans[i] = hitNotes[i];

			ans[newNoteIn12] = true;

			return ans;
		
		}
		List<List<int>^>^ keep_withLoopModeAndInCorrectAmbitus(List<List<int>^>^ serias, char loopMode, List<int>^ interv, int maxAmbitus) {
		
			List<List<int>^>^ ans = gcnew List<List<int>^>;

			for each (List<int>^ seria in serias) {
				if (matchesLoopMode(seria, loopMode, interv) && isInCorrectAmbitus(seria, maxAmbitus)) ans->Add(seria);
			}

			return ans;
		
		}
		bool matchesLoopMode(List<int>^ seria, char loopMode, List<int>^ interv) {
			
			if (loopMode == 'a') {
				// All : convient toujours
				return true;
			} else if (loopMode == 's') {
				// Strict : rechercher si l'intervalle bouclant est un intervalle autorisé
				int first = seria[0];
				int last = seria[11];
				for each (int interval in interv) {
					if (first - last == interval) return true;
				}
				return false;
			} else if (loopMode == 'o') {
				// Octave : rechercher si l'intervalle bouclant est un intervalle autorisé modulo 12
				int first = seria[0];
				int last = seria[11];
				for each (int interval in interv) {
					if ((first - last - interval + 1200) % 12 == 0) return true;
				}
				return false;
			} else {
				throw gcnew Exception;
			}

		}
		bool isInCorrectAmbitus(List<int>^ seria, int maxAmbitus) {
		
			int min(9999999), max(-9999999);
			for each (int note in seria) {
				if (note < min) min = note;
				if (note > max) max = note;
			}

			return (max - min) <= maxAmbitus;
		
		}
		List<List<int>^>^ keep_removeRotations(List<List<int>^>^ serias) {
		
			//
			return serias;
		
		}

};

// Décalage large
public ref class G_Polyrhythm : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Tuplets en décalage";

		}
		virtual String^ description() override {
			
			return "Tuplets massivement décalés.";

		}
		virtual ScoreAndInfos^ generate() override {

			// Variables
			int nbParts = 24;
			int nbNotesFirstPart = 64;
			int nbNotesIncrPerPart = -1;
			int algoIndex = 1;

			// Création
			Block^ block = gcnew Block(nbParts);
			block->addBar(nbNotesFirstPart + "/4");
			for (int part = 1; part <= nbParts; part++) {
				int nbQuartersInThisPart = nbNotesFirstPart + (part - 1) * nbNotesIncrPerPart;
				List<Element^>^ elements = gcnew List<Element^>;
				fillPart(elements, algoIndex, part - 1, nbQuartersInThisPart, nbNotesFirstPart);
				block[part, 1] = gcnew Group(elements->ToArray());
			}

			// Retour
			return block;

		}

	// Algos
	private:
		delegate void Algo(List<Element^>^ elements_out, int partIndexBase0, int nbQuarters, int nbNotesFirstPart);
		static void fillPart(List<Element^>^ elements_out, int algoIndex, int partIndexBase0, int nbQuarters, int nbNotesFirstPart) {
			
			Algo^ fctFill;

			if      (algoIndex == 1) fctFill = gcnew Algo(G_Polyrhythm::fillPart_algo1);
			else if (algoIndex == 2) fctFill = gcnew Algo(G_Polyrhythm::fillPart_algo2);

			fctFill(elements_out, partIndexBase0, nbQuarters, nbNotesFirstPart);

		}
		static void fillPart_algo1(List<Element^>^ elements_out, int partIndexBase0, int nbQuarters, int nbNotesFirstPart) {
				
			List<NoteOrRest^>^ notes = gcnew List<NoteOrRest^>;
			
			for (int i = 0; i < nbQuarters; i++) {
				notes->Add(qNote("q|" + (72 - partIndexBase0)));
			}
			
			Tuplet^ tuplet = gcnew Tuplet(gcnew TupletRatio(RhythmFigure::_quarter, nbQuarters, nbNotesFirstPart), notes);
			elements_out->Add(tuplet);

		}
		static void fillPart_algo2(List<Element^>^ elements_out, int partIndexBase0, int nbQuarters, int nbNotesFirstPart) {
				
			List<NoteOrRest^>^ notes = gcnew List<NoteOrRest^>;

			for (int i = 0; i < nbQuarters; i++) {
				if (Rand::double01() < 0.85) {
					notes->Add(qNote("q|" + (72 - partIndexBase0)));
				} else {
					notes->Add(qRest("q"));
				}
			}
			
			Tuplet^ tuplet = gcnew Tuplet(gcnew TupletRatio(RhythmFigure::_quarter, nbQuarters, nbNotesFirstPart), notes);
			elements_out->Add(tuplet);

		}

};
