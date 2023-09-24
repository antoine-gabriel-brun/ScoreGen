#pragma region head

	#pragma once

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	#include "OptionsForm.h"

#pragma endregion

namespace ScoreGen {
	public ref class OF_TestContraintes : public OptionsForm {

	// Constructeur, destructeur
	public:
		OF_TestContraintes(String^ title, String^ description) :
			OptionsForm(title, description)
		{
			
			String^ text = Text;

			InitializeComponent();

			Text = text;

		}
	protected:
		~OF_TestContraintes() {

			if (components) delete components;

		}

	// Implémentations
	protected:
		virtual array<Object^, 1>^ getOptionsInFormValues() override {

			return gcnew array<Object^, 1>{
				calcNotes((int)(numericUpDownNbNotes->Value))
			};

		}

	// Calcul des notes
	private:
		static array<int, 2>^ calcNotes(int nbNotes) {
			
			array<int, 2>^ ans = gcnew array<int, 2>(4, nbNotes);

			// Choix des notes
			double progress;
			for (int i = 0; i < nbNotes; i++) {
				progress = ((double)i) / nbNotes;
				// Type d'accord : aléatoire parmi plusieurs modèles prédéfinis, et calcul des notes minimum et maximum possibles pour cet accord
				array<int, 1>^ chord = chooseChordType(progress);
				int transpMin, transpMax; { auto minmax = calcTranspMinTranspMax(chord, progress); transpMin = minmax[0]; transpMax = minmax[1]; }
				// Choisir la transposition (en évitant toute note répétée par rapport à l'accord précédent, et les grands sauts)
				do {
					// Enregistrer l'accord à l'index (i) avec une transposition donnée
					writeChordWithRandomTransposition(ans, i, chord, transpMin, transpMax);
					// Si cet accord convient à cette transposition, c'est ok et on arrête la boucle
					if (newChordWithThisTranspositionIsOk(ans, i, progress)) break;
				} while (true);
			}

			// Retour
			return ans;

		}
	private:
		static array<array<int, 1>^, 1>^ chords = gcnew array<array<int, 1>^, 1> {
			gcnew array<int, 1> {0, 5, 9, 11},
			gcnew array<int, 1> {0, 2, 7, 11},
			gcnew array<int, 1> {0, 4, 9, 13},
			gcnew array<int, 1> {0, 5, 11, 15},
			gcnew array<int, 1> {0, 9, 13, 18}
		};
		static array<int, 1>^ notesMinInstruments = gcnew array<int, 1> { 55, 55, 48, 36 };
		static array<int, 1>^ notesMaxInstruments = gcnew array<int, 1> { 93, 90, 86, 76 };
		static array<int, 1>^ chooseChordType(double progress) {
			
			if      (progress < 0.6 ) return Rand::chooseIn(chords, 0, 3);
			else if (progress < 0.85) return Rand::chooseIn(chords, 0, 4);
			else                      return Rand::chooseIn(chords, 0, 5);

		}
		static array<int, 1>^ calcTranspMinTranspMax(array<int, 1>^ chord, double progress) {

			int transpMin = 0, transpMax = 1000;

			// Calculer le min et le max naïvement
			for (int instr = 0; instr < 4; instr++) {
				transpMin = Math::Max(transpMin, notesMinInstruments[instr] - chord[3 - instr]);
				transpMax = Math::Min(transpMax, notesMaxInstruments[instr] - chord[3 - instr]);
			}

			// Evoluer en fonction de l'avancement : partir grave, élargir, finir aigu
			double _min01, _max01; int new_transpMin, new_transpMax; {
				if (progress < 0.1) {
					_min01 = 0;
					_max01 = 0.5;
				} else if (progress < 0.6) {
					_min01 = 0;
					_max01 = progress + 0.4;
				} else {
					_min01 = 1.5 * (progress - 0.6);
					_max01 = 1;
				}
				new_transpMin = (int)Math::Round(transpMin + _min01 * (transpMax - transpMin));
				new_transpMax = (int)Math::Round(transpMin + _max01 * (transpMax - transpMin));
			}
			transpMin = new_transpMin;
			transpMax = new_transpMax;

			// Eviter le cas min > max
			if (transpMin > transpMax) transpMin = transpMax = (transpMin + transpMax) / 2;

			// Retourner
			return gcnew array<int, 1> {transpMin, transpMax};

		}
		static void writeChordWithRandomTransposition(array<int, 2>^ out_notes, int i, array<int, 1>^ chord, int transpMin, int transpMax) {
			
			int transp = Rand::integer(transpMin, transpMax + 1);
			
			for (int instr = 0; instr < 4; instr++) {
				out_notes[instr, i] = transp + chord[3 - instr];
			}

		}
		static bool newChordWithThisTranspositionIsOk(array<int, 2>^ notes, int i, double progress) {
			
			// Y a-t-il une note répétée à la même hauteur et au même instrument entre l'accord (i - 1) et l'accord (i) ?
			if (i >= 1) for (int instr = 0; instr < 4; instr++) {
				if (notes[instr, i - 1] == notes[instr, i]) return false;
			}
			
			// Y a-t-il une note répétée à la même hauteur à un instrument extrême entre l'accord (i - 2) et l'accord (i) ?
			if (i >= 2) for (int instr = 0; instr < 4; instr += 3) {
				if (notes[instr, i - 2] == notes[instr, i]) return false;
			}
			
			// Y a-t-il un trop grand saut ?
			int maxJump = (int)Math::Round(Math::Pow(progress, 1.75) * 5 + 4);
			if (i >= 1) for (int instr = 0; instr < 4; instr++) {
				if (Math::Abs(notes[instr, i] - notes[instr, i - 1]) > maxJump) return false;
			}

			// Si aucun problème jusqu'ici, l'accord dans cette transposition convient
			return true;

		}

	// Contrôles
	#pragma region controls
		private: System::Windows::Forms::Label^  label2;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownNbNotes;
		private: System::Windows::Forms::GroupBox^  groupBoxOptions;
		private:
				System::ComponentModel::Container ^components;
	#pragma endregion

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownNbNotes = (gcnew System::Windows::Forms::NumericUpDown());
			this->groupBoxOptions = (gcnew System::Windows::Forms::GroupBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNbNotes))->BeginInit();
			this->groupBoxOptions->SuspendLayout();
			this->SuspendLayout();
			// 
			// label2
			// 
			this->label2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(52, 58);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(125, 17);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Nombre de notes :";
			// 
			// numericUpDownNbNotes
			// 
			this->numericUpDownNbNotes->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownNbNotes->Location = System::Drawing::Point(183, 56);
			this->numericUpDownNbNotes->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1024, 0, 0, 0});
			this->numericUpDownNbNotes->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownNbNotes->Name = L"numericUpDownNbNotes";
			this->numericUpDownNbNotes->Size = System::Drawing::Size(103, 22);
			this->numericUpDownNbNotes->TabIndex = 5;
			this->numericUpDownNbNotes->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {128, 0, 0, 0});
			// 
			// groupBoxOptions
			// 
			this->groupBoxOptions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBoxOptions->Controls->Add(this->numericUpDownNbNotes);
			this->groupBoxOptions->Controls->Add(this->label2);
			this->groupBoxOptions->Location = System::Drawing::Point(12, 84);
			this->groupBoxOptions->Name = L"groupBoxOptions";
			this->groupBoxOptions->Size = System::Drawing::Size(338, 136);
			this->groupBoxOptions->TabIndex = 6;
			this->groupBoxOptions->TabStop = false;
			this->groupBoxOptions->Text = L"Options";
			// 
			// OF_TestContraintes
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(362, 270);
			this->Controls->Add(this->groupBoxOptions);
			this->Name = L"OF_TestContraintes";
			this->Text = L"OF_TestContraintes";
			this->Controls->SetChildIndex(this->groupBoxOptions, 0);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNbNotes))->EndInit();
			this->groupBoxOptions->ResumeLayout(false);
			this->groupBoxOptions->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	
	};
}
