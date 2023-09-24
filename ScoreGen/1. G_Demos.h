#pragma region head

	#pragma once

	#include "ScoreGenLib.h"
	using namespace ScoreGenLib;
	using namespace ScoreGenLib::Generators;
	
	#include "OF_Test.h"
	
#pragma endregion

// Tests, démos, tutos
public ref class G_DemoScale_AllExplicit : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Démo : Gamme (tout explicite)";

		}
		virtual String^ description() override {
			
			return "Exemple d'une gamme chromatique jouée par un quatuor. Chaque note est créée individuellement et explicitement.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			// Créer le bloc
			Block^ block = gcnew Block(4);
			block->addBars(4, "C");

			// Ajouter les parties
			for (int part = 1; part <= 4; part++) {
				// Montée chromatique
				for (int bar = 1; bar <= 3; bar++) {
					Pitch^ pitch1 = gcnew Pitch(72 + 4 * (bar - 1) + 0 - 12 * (part - 1));
					Pitch^ pitch2 = gcnew Pitch(72 + 4 * (bar - 1) + 1 - 12 * (part - 1));
					Pitch^ pitch3 = gcnew Pitch(72 + 4 * (bar - 1) + 2 - 12 * (part - 1));
					Pitch^ pitch4 = gcnew Pitch(72 + 4 * (bar - 1) + 3 - 12 * (part - 1));
					Note^ note1 = gcnew Note(RhythmFigure::_quarter, pitch1, gcnew NoteDecorations(false, bar == 1, false), bar == 1 ? qWords("!mp") : nullptr);
					Note^ note2 = gcnew Note(RhythmFigure::_quarter, pitch2);
					Note^ note3 = gcnew Note(RhythmFigure::_quarter, pitch3);
					Note^ note4 = gcnew Note(RhythmFigure::_quarter, pitch4);
					block[part, bar] = gcnew Group(gcnew array<Element^, 1> { note1, note2, note3, note4 });
				}
				// Ronde finale
				Pitch^ pitch = gcnew Pitch(84 - 12 * (part - 1));
				Note^ note = gcnew Note(RhythmFigure::_whole, pitch, gcnew NoteDecorations(false, false, true));
				block[part, 4] = gcnew Group(gcnew array<Element^, 1> { note });
			}

			// Retourner (conversion implicite Block --> Score --> ScoreAndInfos)
			return block;

		}

};
public ref class G_DemoScale_WithTranspositions : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Démo : Gamme (avec transpositions)";

		}
		virtual String^ description() override {
			
			return "Exemple d'une gamme chromatique jouée par un quatuor. La voix de premier violon est créée, puis recopiée et transposée dans les autres voix.";

		}
		virtual ScoreAndInfos^ generate() override {

			// Créer un groupe contenant toute la partie du premier violon
			Group^ melody = qGroup("q(|C4<!mp> q|C#4 q|D4 q|D#4 q|E4 [3:2 e|F5 e|F5 e|F5] q|F#4 q|G4 q|G#4 q|A4 q|A#4 q|B4 w)|C5");
			
			// Créer le bloc
			Block^ block = gcnew Block(4);
			block->addBars(4, "C");

			// Ajouter chaque partie, en transposant aux octaves inférieurs
			{
				WriteOptions^ writeOptions = gcnew WriteOptions(
					TimeSignaturesOverwriteBehaviour::adaptToCurrentSignatures,
					NotesOverwriteBehaviour         ::requestNoCollisions     ,
					TooMuchPartsBehaviour           ::requestEnoughParts      ,
					TooMuchBarsBehaviour            ::requestEnoughBars
				);
				for (int part = 1; part <= 4; part++) {
					int transpose_octavesDown = (part - 1);
					Group^ transposedMelody = melody->transposition(-12 * transpose_octavesDown);
					block->write(part, 1, transposedMelody, writeOptions);
				}
			}

			// Retourner
			return block;

		}

};
public ref class G_DemoTexts : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Démo : Textes";

		}
		virtual String^ description() override {
			
			return "Exemple avec positionnement de plusieurs types de textes sur les notes et les silences.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			// Créer le bloc
			Block^ block = gcnew Block;

			// Ajouter huit mesures et une partie
			block->addBars(8, "C/");
			block->addParts(1);

			// Ajouter mesures 1 à 5
			block[1, 1] = qGroup("q(|C4<°whole=360><!p> q|D4 q|E4 q)|F4");
			block[1, 2] = qGroup("q(|G4<!ffff><#COMMENT> q|F4<!mp> q|E4 q)|D4<.pizz.>");
			block[1, 3] = qGroup("w|C4<:dolce>");
			block[1, 4] = qGroup("h|C4<!pzz><.une indication !> h|C4");
			block[1, 5] = qGroup("q<.1><:multi-\r\nligne> q<.2> q<.3> q<.4>");

			// Ajouter mesure 6 (texte sur une pause de mesure)
			array<Words::Words^, 1>^ words = newArray(
				qWords("!ffff"),
				qWords(":Texts on pauses are ok")
			);
			block[1, 6] = gcnew Group(gcnew array<NoteOrRest^> { Rest::wholeBarRest(1024, words) });

			// Ajouter mesures 7 et 8 (paroles)
			block[1, 7] = qGroup("q|C5<*Le> q|C5<*si-> q<*-lence> q|C5<*est>");
			block[1, 8] = qGroup("q|C5<*pro-> q|C5<*-non-> q|C5<*-cé...> q|C5|E5<*Oui !>");

			// Retourner
			return block;

		}

};
public ref class G_DemoOptions : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Démo : Options";

		}
		virtual String^ description() override {
			
			return "Test d'un morceau avec paramètres réglables depuis une boîte de dialogue.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			// Récupérer les options
			int nbParts;
			int nbNotes;
			{
				// Montrer la boîte de dialogue
				ScoreGen::OF_Test^ dialog = gcnew ScoreGen::OF_Test(name(), description());
				dialog->ShowDialog();
				// Récupérer les valeurs des options
				array<Object^, 1>^ options = dialog->getOptions();
				if (options == nullptr) return nullptr;
				// Convertir
				nbParts = (int)(options[0]);
				nbNotes = (int)(options[1]);
			}

			// Créer le bloc, et lui ajouter le nombre de parties demandé sur une grande mesure
			Block^ block = gcnew Block;
			block->addParts(nbParts);
			block->addBars(1, gcnew Signature(nbNotes, 4));

			// Ajouter les notes
			for (int part = 1; part <= nbParts; part++) {
				List<Element^>^ elements = gcnew List<Element^>;
				for (int note = 0; note < nbNotes; note++) {
					Pitch^ pitch = gcnew Pitch(66 + nbParts + (3 + note + 2 * part) % 13 - 4 * part);
					elements->Add(gcnew Note(RhythmFigure::_quarter, pitch));
				}
				block[part, 1] = gcnew Group(elements);
			}

			// Retourner
			return block;

		}

};
public ref class G_DemoPercussions : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Démo : Percussions";

		}
		virtual String^ description() override {
			
			return "Exemple de rythmes écrits sur des portées à 1, 2, 3, 4 et 5 lignes.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			// Créer le bloc
			Block^ block = gcnew Block(6);
			block->addBar("C");

			// Ajouter les parties
			block[1, 1] = qGroup("e|*<.percus une ligne :> e|* e|* e|* e|* 16th|* 16th|* [3:2 16th|* 16th|* 16th|*] 32nd|* 32nd|* 32nd|* 32nd|*");
			block[2, 1] = qGroup("q|2/2<.percus deux lignes :> q|1/2 e|2/2 e|2/2 q|1/2");
			block[3, 1] = qGroup("16th@|3/3<.percus trois lignes :> 16th|1/3 16th|2/3 16th@|3/3 16th|1/3 16th|2/3 16th@|1/3 16th|2/3 16th@|3/3 16th|1/3 16th|2/3 16th@|3/3 16th|1/3 16th|2/3 e@|1/3");
			block[4, 1] = qGroup("e|1/4<.percus quatre lignes :> e|2/4 e|3/4 16th|4/4 16th|3/4 16th|2/4 32nd|1/4 32nd|2/4 32nd|3/4 64th|4/4 64th|3/4 64th|2/4 64th|1/4 64th|2/4 64th|3/4 q|4/4");
			block[5, 1] = qGroup("16th|1/5<.percus cinq lignes :> 16th|2/5 16th|3/5 16th|4/5 16th|5/5 16th|4/5 16th|3/5 16th|2/5 h|1/5");
			block[6, 1] = qGroup(
				"[6:4 16th|1/6<.5 lignes\r\n6 valeurs> 16th|2/6 16th|3/6 16th|4/6 16th|5/6 16th|6/6] " +
				"[7:4 16th|1/7<.5 lignes\r\n7 valeurs> 16th|2/7 16th|3/7 16th|4/7 16th|5/7 16th|6/7 16th|7/7] " +
				"[11:4 16th|1/11<.5 lignes\r\n11 valeurs> 16th|2/11 16th|3/11 16th|4/11 16th|5/11 16th|6/11 16th|7/11 16th|8/11 16th|9/11 16th|10/11 16th|11/11] " +
				"[25:4 16th|1/25<.5 lignes\r\n25 valeurs> 16th|2/25 16th|3/25 16th|4/25 16th|5/25 16th|6/25 16th|7/25 16th|8/25 16th|9/25 16th|10/25 16th|11/25 16th|12/25 16th|13/25 16th|14/25 16th|15/25 16th|16/25 16th|17/25 16th|18/25 16th|19/25 16th|20/25 16th|21/25 16th|22/25 16th|23/25 16th|24/25 16th|25/25]"
			);

			// Retourner
			return block;

		}

};
public ref class G_DemoOtherFeatures : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Démo : Autres fonctionnalités";

		}
		virtual String^ description() override {
			
			return "Exemple d'un morceau utilisant de nombreuses possibilités offertes par ScoreGen.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			// Créer le bloc avec quatre parties
			Block^ block = gcnew Block(4);

			// Mesure 1
			block->addBar("C"); {
				block[1, 1] = qGroup("16th|C5<°quarter=160><!f><:en-dehors> e.|G5 q_(|E5 16th_|E5 [3:2 e|E5 e|D5 e|C5] e.)|D5");
				block[2, 1] = qGroup("h...|C5|C6<!f> 16th|B4");
				block[3, 1] = qGroup("h.|E4<!f> q|D4");
				block[4, 1] = qGroup("h.(|C4<!f> q)|G3");
			}

			// Mesure 2
			block->addBar("2/4"); {
				block[1, 2] = qGroup("[q:3:2 16th|G4<#La mesure 2\r\ncommence ici> e. e|A(b)4 e e|Cb(b)5 e]");
				block[2, 2] = qGroup("q(|G4 q|F4");
				block[3, 2] = qGroup("[3:2 q q|D4 q|D4]");
				// (block[4, 2] reste vide)
			}

			// Mesure 3
			block->addBar("4/1"); {
				block[1, 3] = qGroup("b|C5<.pizz.> w|C5 h|C5 q|C5 e|C5 16th|C5 32nd|C5 64th|C5 128th|C5 256th|C5 256th");
				block[2, 3] = qGroup("b)|E4<.pizz.> w..._|E4 16th_|E4 32nd...|E4 256th");
				block[3, 3] = qGroup("b|G3<.pizz.> w|G#3 h|G##3 q|Gb3 e|Gbb3 16th|G3 32nd|G3 64th|G3 128th|G3 256th|G3 256th");
				block[4, 3] = qGroup("b|C2|C3<.pizz.> w... 16th 32nd... 256th");
			}

			// Mesure 4 : vide et composée
			block->addBar("3+3+1/16");

			// Mesure 5 : tuplets
			block->addBar("3/4"); {
				block[1, 5] = qGroup("[q:4:3 q|B4 q|B4 q|B4 q|B4]");
				block[2, 5] = qGroup("[q:5:3 q|B4 q|B4 q|B4 q|B4 q|B4]");
				block[3, 5] = qGroup("[q.:3:2 q.|C4 q.|C4 q.|C4]");
				block[4, 5] = qGroup("[e:15:6 e e e e e e e e e e e e e e e]");
			}

			// Mesure 6 : tied, grace notes, articulations
			block->addBar("8/4"); {
				block[1, 6] = qGroup("q_(|C4 q_|C4 q|C4 q_|C4 [5:4 q_|C4 q_|C4 q q_|C4 q)|C4]");
				block[2, 6] = qGroup("16th,|C5 q|D5 16th,|C5|E5 q|D5|F5 16th,|A4 16th,|B4|D5 q|C5|E5|G5 e 32nd,|E5|G5|C6 32nd, 32nd,|E5|G5|C6 e|E5|G5|C6 w");
				List<Element^>^ elements = gcnew List<Element^>;
				auto pitches = gcnew array<Pitch^> { qPitch("G4") };
				elements->Add(qElement("e@|G4")); // accent
				elements->Add(qElement("e^|G4")); // strong-accent
				elements->Add(qElement("e'|G4")); // staccato
				elements->Add(qElement("e-|G4")); // tenuto
				elements->Add(gcnew Note(RhythmFigure::_eighth, pitches, Articulation::detachedLegato));
				elements->Add(gcnew Note(RhythmFigure::_eighth, pitches, Articulation::staccatissimo ));
				elements->Add(gcnew Note(RhythmFigure::_eighth, pitches, Articulation::spiccato      ));
				elements->Add(gcnew Note(RhythmFigure::_eighth, pitches, Articulation::scoop         ));
				elements->Add(gcnew Note(RhythmFigure::_eighth, pitches, Articulation::plop          ));
				elements->Add(gcnew Note(RhythmFigure::_eighth, pitches, Articulation::doit          ));
				elements->Add(gcnew Note(RhythmFigure::_eighth, pitches, Articulation::falloff       ));
				elements->Add(gcnew Note(RhythmFigure::_eighth, pitches, Articulation::breathMark    ));
				elements->Add(gcnew Note(RhythmFigure::_eighth, pitches, Articulation::caesura       ));
				elements->Add(gcnew Note(RhythmFigure::_eighth, pitches, Articulation::stress        ));
				elements->Add(gcnew Note(RhythmFigure::_eighth, pitches, Articulation::unstress      ));
				elements->Add(gcnew Note(RhythmFigure::_eighth, pitches, gcnew NoteDecorations(gcnew array<Articulation^, 1> {
					Articulation::accent,
					Articulation::spiccato,
					Articulation::tenuto,
					Articulation::scoop,
					Articulation::falloff,
					Articulation::breathMark
				})));
				block[3, 6] = gcnew Group(elements);
				block[4, 6] = qGroup("q@|D2 q@|F2 q@|A2 q@|C3 q@|E3 q@|G3 q@|B3 q@|D4");
			}

			// Retourner
			return gcnew Score(block, newArray(Clef::G2, Clef::G2, Clef::C3, Clef::F4));

		}

};
public ref class G_DemoCloudMaker : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Démo : Nuage automatique";

		}
		virtual String^ description() override {
			
			return "Exemple de nuage rythmique formé sous contraintes : trois types de motifs, densité 0.2 --> 16.";

		}
		virtual ScoreAndInfos^ generate() override {

			// Créer le nuage à l'aide d'un CloudMaker (trois motifs, 16 mesures en doubles-croches, densité exponentielle 0.2 --> 16)
			array<CloudMaker::PatternWithPos^, 1>^ cloud = getCloud();

			// Créer un bloc représentant ce nuage
			Block^ block = CloudMaker::CloudMaker::cloudToBlock(cloud);

			// Retourner (conversion implicite Block --> Score --> ScoreAndInfos)
			return block;

		}

	// Fonctions
	private:
		array<CloudMaker::PatternWithPos^, 1>^ getCloud() {

			// Créer le CloudMaker : 16 mesures en doubles-croches, densité exponentielle 0.2 --> 16
			CloudMaker::CloudMaker^ cloudMaker = gcnew CloudMaker::CloudMaker(256, RhythmFigure::_16th, gcnew TimeVariable_Exp(0.2, 16));

			// Ajouter trois motifs possibles : deux noires, quatre croches, trois doubles
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CloudMaker::Pattern(gcnew array<Element^, 1> {
					qElement("q(|C4"),
					qElement("q)|D4")
				}, CloudMaker::PatternPossiblePosition::onlyOnQuarter), // plaçable sur les noires
				gcnew TimeVariable_Lin(1, 0) // présent au début puis de moins en moins souvent
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CloudMaker::Pattern(gcnew array<Element^, 1> {
					qElement("e(|C4"),
					qElement("e|D4"),
					qElement("e|C4"),
					qElement("e)|D4")
				}, CloudMaker::PatternPossiblePosition::onlyOnEighth), // plaçable sur les croches
				gcnew TimeVariable_Const(1) // présent de façon constante
			));
			cloudMaker->addPattern(gcnew CloudMaker::PatternWithProba(
				gcnew CloudMaker::Pattern(gcnew array<Element^, 1> {
					qElement("16th(|C4"),
					qElement("16th|D4"),
					qElement("16th)|G4")
				}, CloudMaker::PatternPossiblePosition::onlyOn16th), // plaçable sur les doubles-croches
				gcnew TimeVariable_Lin(0, 1) // non présent au début puis présent de plus en plus souvent
			));

			// Retourner
			return cloudMaker->createCloud();

		}

};
public ref class G_DemoMarkovOnNotes : public Generator {

	// Options
	private:
		static int nbNotes = 400;
		static RhythmFigure^ rhythmFigure = RhythmFigure::_16th;
		static int notesPerBar = 16;

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Démo : Chaîne de Markov évolutive sur une série de notes";

		}
		virtual String^ description() override {
			
			return "Le but est de parcourir une échelle chromatique du bas vers le haut, en utilisant une chaîne de Markov évolutive.";

		}
		virtual ScoreAndInfos^ generate() override {

			// Créer la chaîne de Markov
			Markov^ chain = getMarkovChainInitialized();

			// Créer le mode (chromatique) sur lequel indicer la chaîne de Markov
			int order = chain->order;
			array<Pitch^, 1>^ pitches = gcnew array<Pitch^, 1>(order);
			for (int i = 0; i < order; i++) pitches[i] = gcnew Pitch(60 + i);

			// Générer les notes en utilisant la chaîne de Markov
			List<Element^>^ elements = gcnew List<Element^>; {
				for (int i = 0; i < nbNotes; i++) {
					Pitch^ pitch = pitches[chain->next(i, nbNotes)];
					Element^ element = gcnew Note(rhythmFigure, pitch);
					elements->Add(element);
				}
			}

			// Répartir les notes dans les mesures
			List<List<Element^>^>^ elements_bars = gcnew List<List<Element^>^>; {
				int currentNb = 0;
				List<Element^>^ currentList = gcnew List<Element^>;
				elements_bars->Add(currentList);
				for each (Element^ element in elements) {
					if (currentNb == notesPerBar) {
						currentNb = 0;
						currentList = gcnew List<Element^>;
						elements_bars->Add(currentList);
					}
					currentNb++;
					currentList->Add(element);
				}
			}

			// Créer et remplir le block
			Block^ ans = gcnew Block(1); {
				int cptBar = 0;
				for each (List<Element^>^ barContents in elements_bars) {
					cptBar++;
					Signature^ timeSignature = gcnew Signature(barContents->Count * rhythmFigure->duration_1024th, nullptr);
					ans->addBar(timeSignature);
					ans[1, cptBar] = gcnew Group(barContents);
				}
			}

			// Retourner
			return ans;

		}

	// Fonctions
	private:
		Markov^ getMarkovChainInitialized() {
			
			int order = 12;

			// Créer les probabilités évolutives
			array<double, 2>^ probas_beg = gcnew array<double, 2> {
				{ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			};
			array<double, 2>^ probas_end = gcnew array<double, 2> {
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 }
			};
			array<TimeVariable^, 2>^ probas = gcnew array<TimeVariable^, 2>(order, order);
			for (int i = 0; i < order; i++) {
				for (int j = 0; j < order; j++) {
					probas[i, j] = gcnew TimeVariable_Sigm(probas_beg[i, j], probas_end[i, j], 1.5);
				}
			}

			// Retourner la chaîne de Markov initialisée
			Markov^ ans = gcnew Markov(order, probas, false);
			ans->initialize(0);
			return ans;

		}

};
public ref class G_DemoMarkovOnBlocks : public G_MarkovOnBlocks {

	// Implémentations
	public:
		// Générateur
		virtual String^ name() override {
			
			return "Démo : Chaîne de Markov évolutive sur des blocks";

		}
		virtual String^ description() override {
			
			return "Chaîne de Markov décrivant le déroulement rythmique et motivique de la musique.";

		}
	protected:
		// Chaîne de Markov
		virtual int getOrder() override {
			
			return 5;

		}
		virtual List<TimeAndMatrix^>^ getProbaStages() override {
			
			List<TimeAndMatrix^>^ ans = gcnew List<TimeAndMatrix^>;

			ans->Add(gcnew TimeAndMatrix(
				0, gcnew array<double, 2> { // début : 1 - 4 - 2 - 3 - 1
				{ 0, 0, 0, 1, 0 },
				{ 0, 0, 1, 0, 0 },
				{ 1, 0, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 }
			}));
			ans->Add(gcnew TimeAndMatrix(
				0.2, gcnew array<double, 2> { // 0.2 : 2 - 3 - 4
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 1, 0, 0 },
				{ 0, 0, 0, 1, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 }
			}));
			ans->Add(gcnew TimeAndMatrix(
				0.5, gcnew array<double, 2> { // 0.5 : 2 - 5
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 1 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 1, 0, 0, 0 }
			}));
			ans->Add(gcnew TimeAndMatrix(
				0.8, gcnew array<double, 2> { // 0.8 : 2 - 3 - 4
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 1, 0, 0 },
				{ 0, 0, 0, 1, 0 },
				{ 0, 1, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 }
			}));
			ans->Add(gcnew TimeAndMatrix(
				1, gcnew array<double, 2> { // fin : 3 - 2 - 5
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 1 },
				{ 0, 1, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 1, 0, 0 }
			}));

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
			
			int quarters = 80;

			return 256 * quarters;

		}
		virtual int getFinalValue_or_m1() override {
			
			return 4;

		}
		virtual bool getSquareProbas() override {

			return false;

		}
		virtual ScoreAndInfos^ doOnGeneratedBlock(Block^ block) override {
			
			return block;

		}

	// Classes de chaque générateur
	private:
		ref class BlocksGenerator0 : public BlocksGeneratorForMarkov {

			// Implémentation
			public:
				virtual Block^ generate(double time01, int indexInGlobalSeq, int indexOfThisType) override {
					
					Block^ block = gcnew Block(3);
					block->addBar(qSignature("1/8"));

					block[1, 1] = qGroup("e'@|G5");
					block[2, 1] = qGroup("e'@|E4");
					block[3, 1] = qGroup("e'@|C4");

					return block;

				}
		
		};
		ref class BlocksGenerator1 : public BlocksGeneratorForMarkov {

			// Implémentation
			public:
				virtual Block^ generate(double time01, int indexInGlobalSeq, int indexOfThisType) override {
					
					Block^ block = gcnew Block(3);
					block->addBar(qSignature("1/8"));

					block[1, 1] = qGroup("e'|A4");
					block[2, 1] = qGroup("e'|E4");
					block[3, 1] = qGroup("e'|C4");

					return block;

				}
		
		};
		ref class BlocksGenerator2 : public BlocksGeneratorForMarkov {

			// Implémentation
			public:
				virtual Block^ generate(double time01, int indexInGlobalSeq, int indexOfThisType) override {
					
					Block^ block = gcnew Block(3);
					block->addBar(qSignature("1/8"));

					block[1, 1] = qGroup("16th(|Bb4 16th)|Ab4");
					block[2, 1] = qGroup("e'|F4");
					block[3, 1] = qGroup("e'|D4");

					return block;

				}
		
		};
		ref class BlocksGenerator3 : public BlocksGeneratorForMarkov {

			// Implémentation
			public:
				virtual Block^ generate(double time01, int indexInGlobalSeq, int indexOfThisType) override {
					
					Block^ block = gcnew Block(3);
					block->addBar(qSignature("1/4"));

					block[1, 1] = qGroup("[3:2 e(|Ab4 e|F4 e)|Gb4]");
					block[2, 1] = qGroup("q-|D4");
					block[3, 1] = qGroup("q-|B3");

					return block;

				}
		
		};
		ref class BlocksGenerator4 : public BlocksGeneratorForMarkov {

			// Implémentation
			public:
				virtual Block^ generate(double time01, int indexInGlobalSeq, int indexOfThisType) override {
					
					Block^ block = gcnew Block(3);
					block->addBar(qSignature("1/8"));

					block[1, 1] = qGroup("e'@|B4");
					block[2, 1] = qGroup("e'@|A4");
					block[3, 1] = qGroup("e'@|G4");

					return block;

				}
		
		};

};
public ref class G_DemoScale : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Démo : gamme avec échanges";

		}
		virtual String^ description() override {
			
			return "Gamme chromatique avec quelques échanges locaux.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			array<int>^ pitches = newArray(60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72);
			
			for (int i = 0; i < 10; i++) {
				int pos = Rand::integer(0, 10);
				int exchange = pitches[pos];
				pitches[pos] = pitches[pos + 1];
				pitches[pos + 1] = exchange;
			}

			Block^ block = gcnew Block(1);
			block->addBar("13/4");

			List<Element^>^ contents = gcnew List<Element^>;
			for each (int pitch in pitches) contents->Add(gcnew Note(RhythmFigure::_quarter, pitch));
			block[1, 1] = gcnew Group(contents);

			return block;

		}

};
public ref class G_DemoCut : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Démo : coupure d'éléments";

		}
		virtual String^ description() override {
			
			return "Eléments coupé au niveau des barres de mesures.";

		}
		virtual ScoreAndInfos^ generate() override {
			
			// Quelques groupes plus ou moins complexes
			array<Group^, 1>^ groupsToCut = newArray(
				qGroup("q@|B4"),
				qGroup("q"),
				qGroup("e.(|G4 16th)|C5"),
				qGroup("[3:2 e(|C4 e')|D4 e'|E4]"),
				qGroup("[3:2 e e e]"),
				qGroup("[5:4 16th|C4 16th|D4 16th|E4 16th|F4 16th|G4]"),
				qGroup("[5:4 16th(|C4 16th 16th_|D4 16th|D4 16th)|G4]"),
				qGroup("[4:2 e(|C4 e)|D4 e(|E4 e)|F4]"),
				qGroup("[16th:7:4 16th.(|C4 32nd|D4 16th|E4 16th|F4 16th|G4 16th|A4 16th)|B4]"),
				qGroup("[3:2 e(|A4<.A> e)|B4<.B> e<.C>]")
			);

			// Signatures rythmiques des mesures où ajouter ces groupes de façon répétée
			array<Signature^, 1>^ timeSignatures = newArray(
				qSignature("2/4"),
				qSignature("3/8"),
				qSignature("1/8"),
				qSignature("9/16"),
				qSignature("7/16"),
				qSignature("5/64"),
				qSignature("11/64"),
				qSignature("1/64"),
				qSignature("15/64"),
				qSignature("15/64"),
				qSignature("1/64")
			);

			// Créer la grille
			int nbParts = groupsToCut->Length;
			int nbBars = timeSignatures->Length;
			array<List<Element^>^, 2>^ grid = gcnew array<List<Element^>^, 2>(nbParts, nbBars); {
				for (int part = 0; part < nbParts; part++) {
					for (int bar = 0; bar < nbBars; bar++) {
						grid[part, bar] = gcnew List<Element^>;
					}
				}
			}

			// Remplir chaque partie
			for (int part = 0; part < nbParts; part++) {
				fillPartInGrid(grid, part, groupsToCut[part], nbBars, timeSignatures);
			}

			// Convertir en block
			Block^ block = gcnew Block(nbParts); {
				for (int bar = 1; bar <= nbBars; bar++) {
					block->addBar(timeSignatures[bar - 1]);
					for (int part = 1; part <= nbParts; part++) {
						block[part, bar] = gcnew Group(grid[part - 1, bar - 1]);
					}
				}
			}

			// Retourner
			return block;

		}

	// Fonctions
	private:
		void fillPartInGrid(array<List<Element^>^, 2>^ grid, int part, Group^ groupToCut, int nbBars, array<Signature^, 1>^ timeSignatures) {

			// Parcourir les mesures en remplissant
			array<Element^, 1>^ remainder = gcnew array<Element^, 1>(0);
			for (int bar = 0; bar < nbBars; bar++) {
				// Ajouter le reste laissé par la mesure précédente
				for each (Element^ element in remainder) grid[part, bar]->Add(element);
				// Calculer le nombre de groupes casables sans dépasser, et le temps restant
				int durRemainder_1024th = 0; for each (Element^ element in remainder) durRemainder_1024th += element->duration_1024th;
				int durToFill_1024th = timeSignatures[bar]->duration_1024th - durRemainder_1024th;
				int durGroupToCut_1024th = groupToCut->duration_1024th;
				int nbEntireGroups = durToFill_1024th / durGroupToCut_1024th;
				int durIncompleteGroup_1024th = durToFill_1024th - nbEntireGroups * durGroupToCut_1024th;
				// Ajouter les groupes entiers
				for (int i = 0; i < nbEntireGroups; i++) for each (Element^ element in groupToCut->elements) grid[part, bar]->Add(element);
				// S'il y a un groupe à couper à la fin de la mesure, effectuer la découpe
				LeftRight<Group^>^ leftRight; {
					if (durIncompleteGroup_1024th == 0) {
						// Pas de groupe à couper
						leftRight = gcnew LeftRight<Group^>(
							gcnew Group(gcnew array<Element^, 1>(0)),
							gcnew Group(gcnew array<Element^, 1>(0))
						);
					} else {
						// Couper le groupe
						leftRight = groupToCut->cutAt(durIncompleteGroup_1024th);
					}
				}
				// Ajouter la partie gauche pour compléter la mesure, et garder le reste pour la mesure suivante
				for each (Element^ element in leftRight->left->elements) grid[part, bar]->Add(element);
				remainder = leftRight->right->elements;
			}

		}

};
public ref class G_DemoLSystem : public Generator {

	// Implémentations
	public:
		virtual String^ name() override {
			
			return "Démo : L-système";

		}
		virtual String^ description() override {
			
			return "L-système proposé par Martin Supper dans 'A Few Remarks on Algorithmic Composition'.";

		}
		virtual ScoreAndInfos^ generate() override {
			// Séquence
			int nbElements = 12;
			array<int, 1>^ seq;
			int len;
			int iterations = 16;
			{
				LSystem^ ls = gcnew LSystem(nbElements);
				ls->setRule( 0, gcnew array<int, 1>{1, 0});
				ls->setRule( 1, gcnew array<int, 1>{3, 2});
				ls->setRule( 2, gcnew array<int, 1>{3, 10, 4, 11});
				ls->setRule( 3, gcnew array<int, 1>{3});
				ls->setRule( 4, gcnew array<int, 1>{5, 6});
				ls->setRule( 5, gcnew array<int, 1>{3, 7});
				ls->setRule( 6, gcnew array<int, 1>{5, 8});
				ls->setRule( 7, gcnew array<int, 1>{3, 10, 9, 11});
				ls->setRule( 8, gcnew array<int, 1>{5, 0});
				ls->setRule( 9, gcnew array<int, 1>{3, 9});
				ls->setRule(10, gcnew array<int, 1>{10});
				ls->setRule(11, gcnew array<int, 1>{11});
				seq = ls->generate(4, iterations);
				len = seq->Length;
			}
			// Eléments
			array<Group^, 1>^ elements = gcnew array<Group^, 1> {
				qGroup("16th|60"),
				qGroup("16th|61"),
				qGroup("16th|62"),
				qGroup("16th|63"),
				qGroup("16th|64"),
				qGroup("16th|65"),
				qGroup("16th|66"),
				qGroup("16th|67"),
				qGroup("16th|68"),
				qGroup("16th|69"),
				qGroup("16th|70"),
				qGroup("16th|71")
			};
			// Synthèse
			List<Element^>^ elementsSeq; {
				elementsSeq = gcnew List<Element^>;
				for each (int index in seq) {
					for each (Element^ element in elements[index]->elements) {
						elementsSeq->Add(element);
					}
				}
			}
			// Partition
			Group^ ans = gcnew Group(elementsSeq);
			return gcnew Score(ans);
		}

};
