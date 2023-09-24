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
	public ref class OF_Combinations : public OptionsForm {

	// Constructeur, destructeur
	public:
		OF_Combinations(String^ title, String^ description) :
			OptionsForm(title, description)
		{
			
			String^ text = Text;

			InitializeComponent();

			Text = text;

		}
	protected:
		~OF_Combinations() {

			if (components) delete components;

		}

	// Evénements
	private:
		void OF_Combinations_Load(Object^, EventArgs^) {
			
			comboBoxRhythm->SelectedIndex = 2;
			
		}
		void buttonChroma_Click(Object^, EventArgs^) {
		
			int n = (int)(numericUpDownN->Value);
			int basePitch = 60;

			String^ ans = "";
			for (int i = 0; i < n; i++) {
				int pitch = basePitch + i;
				if (pitch < 0 || pitch > 127) throw gcnew Exception;
				int note = pitch % 12;
				int octave = pitch / 12 - 1;
				String^ note_str;
				if      (note ==  0) note_str = "C";
				else if (note ==  1) note_str = "C#";
				else if (note ==  2) note_str = "D";
				else if (note ==  3) note_str = "Eb";
				else if (note ==  4) note_str = "E";
				else if (note ==  5) note_str = "F";
				else if (note ==  6) note_str = "F#";
				else if (note ==  7) note_str = "G";
				else if (note ==  8) note_str = "G#";
				else if (note ==  9) note_str = "A";
				else if (note == 10) note_str = "Bb";
				else                 note_str = "B";
				ans += " " + note_str + octave;
			}

			textBoxModeNotes->Text = ans->TrimStart();

		}
		void radioButtonMusicalTypeFigures_CheckedChanged(Object^, EventArgs^) {
			
			checkBoxSlurs->Enabled = radioButtonMusicalTypeFigures->Checked;
			
		}

	// Implémentations
	protected:
		virtual array<Object^, 1>^ getOptionsInFormValues() override {

			return gcnew array<Object^, 1>{
				(array<String^, 1>^)(getMode(textBoxModeNotes->Text)       ),
				(int               )(numericUpDownN->Value                 ),
				(int               )(numericUpDownP->Value                 ),
				(int               )(getRhythm_1024th(comboBoxRhythm->Text)),
				(int               )(numericUpDownBarsLen->Value           ),
				(bool              )(checkBoxAlsoSmaller->Checked          ),
				(int               )(getSorting()                          ),
				(char              )(getMusicalFiguresType()               ),
				(bool              )(checkBoxSlurs->Checked                )
			};

		}
	private:
		static array<String^, 1>^ getMode(String^ s) {
			
			return s->Replace("\r", " ")->Replace("\n", " ")->Replace("\t", " ")->Split(gcnew array<wchar_t, 1> { ' ' }, StringSplitOptions::RemoveEmptyEntries);

		}
		static int getRhythm_1024th(String^ s) {
			
			String^ val = s->ToLower()->Replace(" ", "");

			if      (val == "noires"        ) return 256;
			else if (val == "croches"       ) return 128;
			else if (val == "doublescroches") return  64;
			else if (val == "triplescroches") return  32;
			else                              throw gcnew Exception;

		}
		int getSorting() {
			
			if      (radioButtonSorting1->Checked) return 1;
			else if (radioButtonSorting2->Checked) return 2;
			else if (radioButtonSorting3->Checked) return 3;
			else                                   throw gcnew Exception;

		}
		char getMusicalFiguresType() {
			
			if      (radioButtonMusicalTypeChords ->Checked) return 'c';
			else if (radioButtonMusicalTypeFigures->Checked) return 's';
			else                                             throw gcnew Exception;

		}

	// Contrôles
	#pragma region controls
		private: System::Windows::Forms::Label^  label1;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownN;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownP;
		private: System::Windows::Forms::GroupBox^  groupBoxOptions;
		private: System::Windows::Forms::Button^  buttonChroma;
		private: System::Windows::Forms::TextBox^  textBoxModeNotes;
		private: System::Windows::Forms::Label^  label3;
		private: System::Windows::Forms::ComboBox^  comboBoxRhythm;
		private: System::Windows::Forms::Label^  label4;
		private: System::Windows::Forms::CheckBox^  checkBoxAlsoSmaller;
		private: System::Windows::Forms::Label^  label2;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownBarsLen;
		private: System::Windows::Forms::Label^  label5;
		private: System::Windows::Forms::GroupBox^  groupBoxSorting;
		private: System::Windows::Forms::RadioButton^  radioButtonSorting1;
		private: System::Windows::Forms::RadioButton^  radioButtonSorting3;
		private: System::Windows::Forms::RadioButton^  radioButtonSorting2;
		private: System::Windows::Forms::GroupBox^  groupBoxMusicalType;
		private: System::Windows::Forms::RadioButton^  radioButtonMusicalTypeFigures;
		private: System::Windows::Forms::RadioButton^  radioButtonMusicalTypeChords;
		private: System::Windows::Forms::CheckBox^  checkBoxSlurs;
		private: System::ComponentModel::Container ^components;
	#pragma endregion

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->numericUpDownP = (gcnew System::Windows::Forms::NumericUpDown());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownN = (gcnew System::Windows::Forms::NumericUpDown());
			this->groupBoxOptions = (gcnew System::Windows::Forms::GroupBox());
			this->groupBoxMusicalType = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxSlurs = (gcnew System::Windows::Forms::CheckBox());
			this->radioButtonMusicalTypeFigures = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonMusicalTypeChords = (gcnew System::Windows::Forms::RadioButton());
			this->groupBoxSorting = (gcnew System::Windows::Forms::GroupBox());
			this->radioButtonSorting3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonSorting2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonSorting1 = (gcnew System::Windows::Forms::RadioButton());
			this->checkBoxAlsoSmaller = (gcnew System::Windows::Forms::CheckBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownBarsLen = (gcnew System::Windows::Forms::NumericUpDown());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->comboBoxRhythm = (gcnew System::Windows::Forms::ComboBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->buttonChroma = (gcnew System::Windows::Forms::Button());
			this->textBoxModeNotes = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownP))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownN))->BeginInit();
			this->groupBoxOptions->SuspendLayout();
			this->groupBoxMusicalType->SuspendLayout();
			this->groupBoxSorting->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownBarsLen))->BeginInit();
			this->SuspendLayout();
			// 
			// numericUpDownP
			// 
			this->numericUpDownP->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownP->Location = System::Drawing::Point(167, 38);
			this->numericUpDownP->Margin = System::Windows::Forms::Padding(0, 3, 0, 3);
			this->numericUpDownP->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {50, 0, 0, 0});
			this->numericUpDownP->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownP->Name = L"numericUpDownP";
			this->numericUpDownP->Size = System::Drawing::Size(64, 22);
			this->numericUpDownP->TabIndex = 1;
			this->numericUpDownP->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {3, 0, 0, 0});
			// 
			// label1
			// 
			this->label1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(232, 40);
			this->label1->Margin = System::Windows::Forms::Padding(1, 0, 1, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(43, 17);
			this->label1->TabIndex = 2;
			this->label1->Text = L"parmi";
			// 
			// numericUpDownN
			// 
			this->numericUpDownN->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownN->Location = System::Drawing::Point(276, 38);
			this->numericUpDownN->Margin = System::Windows::Forms::Padding(0, 3, 3, 3);
			this->numericUpDownN->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {50, 0, 0, 0});
			this->numericUpDownN->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownN->Name = L"numericUpDownN";
			this->numericUpDownN->Size = System::Drawing::Size(64, 22);
			this->numericUpDownN->TabIndex = 3;
			this->numericUpDownN->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {12, 0, 0, 0});
			// 
			// groupBoxOptions
			// 
			this->groupBoxOptions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBoxOptions->Controls->Add(this->groupBoxMusicalType);
			this->groupBoxOptions->Controls->Add(this->groupBoxSorting);
			this->groupBoxOptions->Controls->Add(this->checkBoxAlsoSmaller);
			this->groupBoxOptions->Controls->Add(this->label2);
			this->groupBoxOptions->Controls->Add(this->numericUpDownBarsLen);
			this->groupBoxOptions->Controls->Add(this->label5);
			this->groupBoxOptions->Controls->Add(this->comboBoxRhythm);
			this->groupBoxOptions->Controls->Add(this->label4);
			this->groupBoxOptions->Controls->Add(this->buttonChroma);
			this->groupBoxOptions->Controls->Add(this->textBoxModeNotes);
			this->groupBoxOptions->Controls->Add(this->label3);
			this->groupBoxOptions->Controls->Add(this->numericUpDownP);
			this->groupBoxOptions->Controls->Add(this->numericUpDownN);
			this->groupBoxOptions->Controls->Add(this->label1);
			this->groupBoxOptions->Location = System::Drawing::Point(12, 84);
			this->groupBoxOptions->Name = L"groupBoxOptions";
			this->groupBoxOptions->Size = System::Drawing::Size(791, 265);
			this->groupBoxOptions->TabIndex = 3;
			this->groupBoxOptions->TabStop = false;
			this->groupBoxOptions->Text = L"Options";
			// 
			// groupBoxMusicalType
			// 
			this->groupBoxMusicalType->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->groupBoxMusicalType->Controls->Add(this->checkBoxSlurs);
			this->groupBoxMusicalType->Controls->Add(this->radioButtonMusicalTypeFigures);
			this->groupBoxMusicalType->Controls->Add(this->radioButtonMusicalTypeChords);
			this->groupBoxMusicalType->Location = System::Drawing::Point(495, 152);
			this->groupBoxMusicalType->Name = L"groupBoxMusicalType";
			this->groupBoxMusicalType->Size = System::Drawing::Size(267, 81);
			this->groupBoxMusicalType->TabIndex = 13;
			this->groupBoxMusicalType->TabStop = false;
			this->groupBoxMusicalType->Text = L"Réalisation musicale";
			// 
			// checkBoxSlurs
			// 
			this->checkBoxSlurs->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBoxSlurs->AutoSize = true;
			this->checkBoxSlurs->Location = System::Drawing::Point(112, 47);
			this->checkBoxSlurs->Name = L"checkBoxSlurs";
			this->checkBoxSlurs->Size = System::Drawing::Size(82, 21);
			this->checkBoxSlurs->TabIndex = 2;
			this->checkBoxSlurs->Text = L"Liaisons";
			this->checkBoxSlurs->UseVisualStyleBackColor = true;
			// 
			// radioButtonMusicalTypeFigures
			// 
			this->radioButtonMusicalTypeFigures->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->radioButtonMusicalTypeFigures->AutoSize = true;
			this->radioButtonMusicalTypeFigures->Checked = true;
			this->radioButtonMusicalTypeFigures->Location = System::Drawing::Point(24, 46);
			this->radioButtonMusicalTypeFigures->Name = L"radioButtonMusicalTypeFigures";
			this->radioButtonMusicalTypeFigures->Size = System::Drawing::Size(82, 21);
			this->radioButtonMusicalTypeFigures->TabIndex = 1;
			this->radioButtonMusicalTypeFigures->TabStop = true;
			this->radioButtonMusicalTypeFigures->Text = L"Arpèges";
			this->radioButtonMusicalTypeFigures->UseVisualStyleBackColor = true;
			this->radioButtonMusicalTypeFigures->CheckedChanged += gcnew System::EventHandler(this, &OF_Combinations::radioButtonMusicalTypeFigures_CheckedChanged);
			// 
			// radioButtonMusicalTypeChords
			// 
			this->radioButtonMusicalTypeChords->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->radioButtonMusicalTypeChords->AutoSize = true;
			this->radioButtonMusicalTypeChords->Location = System::Drawing::Point(24, 19);
			this->radioButtonMusicalTypeChords->Name = L"radioButtonMusicalTypeChords";
			this->radioButtonMusicalTypeChords->Size = System::Drawing::Size(80, 21);
			this->radioButtonMusicalTypeChords->TabIndex = 0;
			this->radioButtonMusicalTypeChords->Text = L"Accords";
			this->radioButtonMusicalTypeChords->UseVisualStyleBackColor = true;
			// 
			// groupBoxSorting
			// 
			this->groupBoxSorting->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->groupBoxSorting->Controls->Add(this->radioButtonSorting3);
			this->groupBoxSorting->Controls->Add(this->radioButtonSorting2);
			this->groupBoxSorting->Controls->Add(this->radioButtonSorting1);
			this->groupBoxSorting->Location = System::Drawing::Point(495, 38);
			this->groupBoxSorting->Name = L"groupBoxSorting";
			this->groupBoxSorting->Size = System::Drawing::Size(267, 108);
			this->groupBoxSorting->TabIndex = 12;
			this->groupBoxSorting->TabStop = false;
			this->groupBoxSorting->Text = L"Ordre";
			// 
			// radioButtonSorting3
			// 
			this->radioButtonSorting3->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->radioButtonSorting3->AutoSize = true;
			this->radioButtonSorting3->Location = System::Drawing::Point(24, 75);
			this->radioButtonSorting3->Name = L"radioButtonSorting3";
			this->radioButtonSorting3->Size = System::Drawing::Size(100, 21);
			this->radioButtonSorting3->TabIndex = 2;
			this->radioButtonSorting3->Text = L"Randomisé";
			this->radioButtonSorting3->UseVisualStyleBackColor = true;
			// 
			// radioButtonSorting2
			// 
			this->radioButtonSorting2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->radioButtonSorting2->AutoSize = true;
			this->radioButtonSorting2->Location = System::Drawing::Point(24, 48);
			this->radioButtonSorting2->Name = L"radioButtonSorting2";
			this->radioButtonSorting2->Size = System::Drawing::Size(237, 21);
			this->radioButtonSorting2->TabIndex = 1;
			this->radioButtonSorting2->Text = L"Apparition progressive des notes";
			this->radioButtonSorting2->UseVisualStyleBackColor = true;
			// 
			// radioButtonSorting1
			// 
			this->radioButtonSorting1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->radioButtonSorting1->AutoSize = true;
			this->radioButtonSorting1->Checked = true;
			this->radioButtonSorting1->Location = System::Drawing::Point(24, 21);
			this->radioButtonSorting1->Name = L"radioButtonSorting1";
			this->radioButtonSorting1->Size = System::Drawing::Size(235, 21);
			this->radioButtonSorting1->TabIndex = 0;
			this->radioButtonSorting1->TabStop = true;
			this->radioButtonSorting1->Text = L"Des plus basses aux plus aiguës";
			this->radioButtonSorting1->UseVisualStyleBackColor = true;
			// 
			// checkBoxAlsoSmaller
			// 
			this->checkBoxAlsoSmaller->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBoxAlsoSmaller->AutoSize = true;
			this->checkBoxAlsoSmaller->Location = System::Drawing::Point(167, 66);
			this->checkBoxAlsoSmaller->Name = L"checkBoxAlsoSmaller";
			this->checkBoxAlsoSmaller->Size = System::Drawing::Size(309, 21);
			this->checkBoxAlsoSmaller->TabIndex = 4;
			this->checkBoxAlsoSmaller->Text = L"Ajouter à la fin les combinaisons plus petites";
			this->checkBoxAlsoSmaller->UseVisualStyleBackColor = true;
			// 
			// label2
			// 
			this->label2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(29, 40);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(104, 17);
			this->label2->TabIndex = 0;
			this->label2->Text = L"Combinaisons :";
			// 
			// numericUpDownBarsLen
			// 
			this->numericUpDownBarsLen->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownBarsLen->Location = System::Drawing::Point(167, 219);
			this->numericUpDownBarsLen->Margin = System::Windows::Forms::Padding(0, 3, 0, 3);
			this->numericUpDownBarsLen->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {256, 0, 0, 0});
			this->numericUpDownBarsLen->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownBarsLen->Name = L"numericUpDownBarsLen";
			this->numericUpDownBarsLen->Size = System::Drawing::Size(64, 22);
			this->numericUpDownBarsLen->TabIndex = 11;
			this->numericUpDownBarsLen->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {12, 0, 0, 0});
			// 
			// label5
			// 
			this->label5->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(29, 221);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(135, 17);
			this->label5->TabIndex = 10;
			this->label5->Text = L"Taille des mesures :";
			// 
			// comboBoxRhythm
			// 
			this->comboBoxRhythm->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->comboBoxRhythm->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBoxRhythm->FormattingEnabled = true;
			this->comboBoxRhythm->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"Noires", L"Croches", L"Doubles croches", 
				L"Triples croches"});
			this->comboBoxRhythm->Location = System::Drawing::Point(167, 189);
			this->comboBoxRhythm->Name = L"comboBoxRhythm";
			this->comboBoxRhythm->Size = System::Drawing::Size(152, 24);
			this->comboBoxRhythm->TabIndex = 9;
			// 
			// label4
			// 
			this->label4->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(29, 192);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(64, 17);
			this->label4->TabIndex = 8;
			this->label4->Text = L"Rythme :";
			// 
			// buttonChroma
			// 
			this->buttonChroma->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->buttonChroma->Location = System::Drawing::Point(399, 159);
			this->buttonChroma->Name = L"buttonChroma";
			this->buttonChroma->Size = System::Drawing::Size(70, 24);
			this->buttonChroma->TabIndex = 7;
			this->buttonChroma->Text = L"Chroma";
			this->buttonChroma->UseVisualStyleBackColor = true;
			this->buttonChroma->Click += gcnew System::EventHandler(this, &OF_Combinations::buttonChroma_Click);
			// 
			// textBoxModeNotes
			// 
			this->textBoxModeNotes->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->textBoxModeNotes->Location = System::Drawing::Point(167, 93);
			this->textBoxModeNotes->Multiline = true;
			this->textBoxModeNotes->Name = L"textBoxModeNotes";
			this->textBoxModeNotes->Size = System::Drawing::Size(302, 90);
			this->textBoxModeNotes->TabIndex = 6;
			this->textBoxModeNotes->Text = L"D6 E5 F5 C6 G5 Db5 B5 Eb6 Ab5 F#6 A6 Bb6";
			// 
			// label3
			// 
			this->label3->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(29, 96);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(102, 17);
			this->label3->TabIndex = 5;
			this->label3->Text = L"Notes (mode) :";
			// 
			// OF_Combinations
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(815, 399);
			this->Controls->Add(this->groupBoxOptions);
			this->Name = L"OF_Combinations";
			this->Text = L"OF_Combinations";
			this->Load += gcnew System::EventHandler(this, &OF_Combinations::OF_Combinations_Load);
			this->Controls->SetChildIndex(this->groupBoxOptions, 0);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownP))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownN))->EndInit();
			this->groupBoxOptions->ResumeLayout(false);
			this->groupBoxOptions->PerformLayout();
			this->groupBoxMusicalType->ResumeLayout(false);
			this->groupBoxMusicalType->PerformLayout();
			this->groupBoxSorting->ResumeLayout(false);
			this->groupBoxSorting->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownBarsLen))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	
	};
}
