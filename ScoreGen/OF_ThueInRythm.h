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
	public ref class OF_ThueInRythm : public OptionsForm {

	// Constructeur, destructeur
	public:
		OF_ThueInRythm(String^ title, String^ description) :
			OptionsForm(title, description)
		{
			
			String^ text = Text;

			InitializeComponent();

			Text = text;

		}
	protected:
		~OF_ThueInRythm() {

			if (components) delete components;

		}

	// Implémentations
	protected:
		virtual array<Object^, 1>^ getOptionsInFormValues() override {

			return gcnew array<Object^, 1> {
				calcNotes(
					(int)(numericUpDownNbNotes    ->Value),
					(int)(numericUpDownDur1       ->Value),
					(int)(numericUpDownDur2       ->Value),
					(int)(numericUpDownInstruments->Value)
				)
			};

		}

	// Calcul des notes
	private:
		static array<bool, 2>^ calcNotes(int nbNotes, int dur1, int dur2, int nbInstruments) {

			// Calculer les 'nbNotes' premiers termes de la suite de Thue-Morse
			array<bool, 1>^ thue = Seq::thueMorse(nbNotes);

			// Calculer l'instant de chacune des notes (à partir de 0), et le nombre d'unités rythmiques nécessaires pour aller jusqu'à la fin
			array<int, 1>^ moments = calcMoments(thue, nbNotes, dur1, dur2);
			int nbRyElements = moments[nbNotes - 1] + 1;
			nbRyElements = 16 * (int)Math::Ceiling(((double)nbRyElements) / 16);

			// Déterminer l'instrument affecté à chaque note
			array<int, 1>^ instruments = calcInstrumentsSequence(nbNotes, nbInstruments);

			// Retourner un tableau de positionnement des notes dans le temps et entre les instruments
			return calcNotesForInstruments(nbInstruments, nbRyElements, nbNotes, moments, instruments);

		}

	// Fonctions
	private:
		static array<int, 1>^ calcMoments(array<bool, 1>^ thue, int nb, int dur1, int dur2) {
			
			array<int, 1>^ ans = gcnew array<int, 1>(nb);

			ans[0] = 0;

			for (int i = 1; i < nb; i++) {
				ans[i] = ans[i - 1] + (thue[i - 1] ? dur2 : dur1);
			}

			return ans;

		}
		static array<int, 1>^ calcInstrumentsSequence(int nbNotes, int nbInstruments) {
			
			array<int, 1>^ ans = gcnew array<int, 1>(nbNotes);

			for (int i = 0; i < nbNotes; i++) {
				ans[i] = Rand::integer(0, nbInstruments);
			}

			return ans;

		}
		static array<bool, 2>^ calcNotesForInstruments(int nbInstruments, int nbRyElements, int nbNotes, array<int, 1>^ moments, array<int, 1>^ instruments) {
			
			array<bool, 2>^ ans = gcnew array<bool, 2>(nbInstruments, nbRyElements);

			for (int i = 0; i < nbNotes; i++) {
				ans[instruments[i], moments[i]] = true;
			}

			return ans;

		}

	// Contrôles
	#pragma region controls
		private: System::Windows::Forms::NumericUpDown^  numericUpDownDur2;
		private: System::Windows::Forms::Label^  label3;
		private: System::Windows::Forms::Label^  label2;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownNbNotes;
		private: System::Windows::Forms::GroupBox^  groupBoxOptions;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownDur1;
		private: System::Windows::Forms::Label^  label1;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownInstruments;
		private: System::Windows::Forms::Label^  label4;
		private:
				System::ComponentModel::Container ^components;
	#pragma endregion

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownNbNotes = (gcnew System::Windows::Forms::NumericUpDown());
			this->groupBoxOptions = (gcnew System::Windows::Forms::GroupBox());
			this->numericUpDownInstruments = (gcnew System::Windows::Forms::NumericUpDown());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownDur2 = (gcnew System::Windows::Forms::NumericUpDown());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownDur1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->label1 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNbNotes))->BeginInit();
			this->groupBoxOptions->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownInstruments))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownDur2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownDur1))->BeginInit();
			this->SuspendLayout();
			// 
			// label2
			// 
			this->label2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(52, 25);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(125, 17);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Nombre de notes :";
			// 
			// numericUpDownNbNotes
			// 
			this->numericUpDownNbNotes->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownNbNotes->Location = System::Drawing::Point(183, 23);
			this->numericUpDownNbNotes->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {20000, 0, 0, 0});
			this->numericUpDownNbNotes->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownNbNotes->Name = L"numericUpDownNbNotes";
			this->numericUpDownNbNotes->Size = System::Drawing::Size(103, 22);
			this->numericUpDownNbNotes->TabIndex = 5;
			this->numericUpDownNbNotes->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
			// 
			// groupBoxOptions
			// 
			this->groupBoxOptions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBoxOptions->Controls->Add(this->numericUpDownInstruments);
			this->groupBoxOptions->Controls->Add(this->label4);
			this->groupBoxOptions->Controls->Add(this->numericUpDownDur2);
			this->groupBoxOptions->Controls->Add(this->label3);
			this->groupBoxOptions->Controls->Add(this->numericUpDownDur1);
			this->groupBoxOptions->Controls->Add(this->label1);
			this->groupBoxOptions->Controls->Add(this->numericUpDownNbNotes);
			this->groupBoxOptions->Controls->Add(this->label2);
			this->groupBoxOptions->Location = System::Drawing::Point(12, 84);
			this->groupBoxOptions->Name = L"groupBoxOptions";
			this->groupBoxOptions->Size = System::Drawing::Size(338, 153);
			this->groupBoxOptions->TabIndex = 6;
			this->groupBoxOptions->TabStop = false;
			this->groupBoxOptions->Text = L"Options";
			// 
			// numericUpDownInstruments
			// 
			this->numericUpDownInstruments->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownInstruments->Location = System::Drawing::Point(183, 107);
			this->numericUpDownInstruments->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownInstruments->Name = L"numericUpDownInstruments";
			this->numericUpDownInstruments->Size = System::Drawing::Size(103, 22);
			this->numericUpDownInstruments->TabIndex = 11;
			this->numericUpDownInstruments->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			// 
			// label4
			// 
			this->label4->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(52, 109);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(89, 17);
			this->label4->TabIndex = 10;
			this->label4->Text = L"Instruments :";
			// 
			// numericUpDownDur2
			// 
			this->numericUpDownDur2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownDur2->Location = System::Drawing::Point(183, 79);
			this->numericUpDownDur2->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {256, 0, 0, 0});
			this->numericUpDownDur2->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownDur2->Name = L"numericUpDownDur2";
			this->numericUpDownDur2->Size = System::Drawing::Size(103, 22);
			this->numericUpDownDur2->TabIndex = 9;
			this->numericUpDownDur2->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {2, 0, 0, 0});
			// 
			// label3
			// 
			this->label3->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(52, 81);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(67, 17);
			this->label3->TabIndex = 8;
			this->label3->Text = L"Durée 2 :";
			// 
			// numericUpDownDur1
			// 
			this->numericUpDownDur1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownDur1->Location = System::Drawing::Point(183, 51);
			this->numericUpDownDur1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {256, 0, 0, 0});
			this->numericUpDownDur1->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownDur1->Name = L"numericUpDownDur1";
			this->numericUpDownDur1->Size = System::Drawing::Size(103, 22);
			this->numericUpDownDur1->TabIndex = 7;
			this->numericUpDownDur1->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			// 
			// label1
			// 
			this->label1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(52, 53);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(67, 17);
			this->label1->TabIndex = 6;
			this->label1->Text = L"Durée 1 :";
			// 
			// OF_ThueInRythm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(362, 287);
			this->Controls->Add(this->groupBoxOptions);
			this->Name = L"OF_ThueInRythm";
			this->Text = L"OF_Echange";
			this->Controls->SetChildIndex(this->groupBoxOptions, 0);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNbNotes))->EndInit();
			this->groupBoxOptions->ResumeLayout(false);
			this->groupBoxOptions->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownInstruments))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownDur2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownDur1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	
	};
}
