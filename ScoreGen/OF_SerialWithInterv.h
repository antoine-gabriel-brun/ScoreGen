#pragma region head

	#pragma once

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Collections;
	using namespace System::Collections::Generic;

	#include "OptionsForm.h"

#pragma endregion

namespace ScoreGen {
	public ref class OF_SerialWithInterv : public OptionsForm {

	// Constructeur, destructeur
	public:
		OF_SerialWithInterv(String^ title, String^ description) :
			OptionsForm(title, description)
		{
			
			String^ text = Text;

			InitializeComponent();

			Text = text;

		}
	protected:
		~OF_SerialWithInterv() {

			if (components) delete components;

		}

	// Implémentations
	protected:
		virtual array<Object^, 1>^ getOptionsInFormValues() override {
			
			List<int>^ intervs = gcnew List<int>; {
				for each (Control^ control in groupBoxOptions->Controls) {
					String^ name = control->Name;
					if (name->StartsWith("checkBox_interv_")) {
						bool plus = name->StartsWith("checkBox_interv_p");
						int interv = Int32::Parse(name->Substring(18));
						CheckBox^ cb = (CheckBox^)control;
						if (cb->Checked) intervs->Add((plus ? 1 : -1) * interv);
					}
				}
			}

			return gcnew array<Object^, 1>{
				intervs,
				(radioButton_loop_all->Checked ? 'a' : (radioButton_loop_strict->Checked ? 's' : 'o')),
				checkBoxRemoveRotations->Enabled && checkBoxRemoveRotations->Checked,
				checkBoxMaxAmbitus->Checked ? ((int)(numericUpDownMaxAmbitus->Value)) : 9999999
			};

		}

	// Evénements
	private:
		void radioButton_loop_all_CheckedChanged(Object^, EventArgs^) {
			 
			 checkBoxRemoveRotations->Enabled = !radioButton_loop_all->Checked;
			 
		}

	// Contrôles
	#pragma region controls
		private: System::Windows::Forms::GroupBox^  groupBoxOptions;
		private: System::Windows::Forms::Label^  label1;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_m_11;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_m_10;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_m_9;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_m_8;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_m_7;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_m_6;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_m_5;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_m_4;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_m_3;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_m_2;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_m_1;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_p_11;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_p_10;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_p_9;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_p_8;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_p_7;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_p_6;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_p_5;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_p_4;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_p_3;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_p_2;
		private: System::Windows::Forms::Label^  label3;
		private: System::Windows::Forms::Label^  label2;
		private: System::Windows::Forms::CheckBox^  checkBox_interv_p_1;

	private: System::Windows::Forms::CheckBox^  checkBoxRemoveRotations;
	private: System::Windows::Forms::GroupBox^  groupBoxLoopMode;
	private: System::Windows::Forms::RadioButton^  radioButton_loop_octave;

	private: System::Windows::Forms::RadioButton^  radioButton_loop_strict;


	private: System::Windows::Forms::RadioButton^  radioButton_loop_all;
	private: System::Windows::Forms::NumericUpDown^  numericUpDownMaxAmbitus;
	private: System::Windows::Forms::CheckBox^  checkBoxMaxAmbitus;

	private: System::ComponentModel::Container ^components;
	#pragma endregion

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->groupBoxOptions = (gcnew System::Windows::Forms::GroupBox());
			this->numericUpDownMaxAmbitus = (gcnew System::Windows::Forms::NumericUpDown());
			this->groupBoxLoopMode = (gcnew System::Windows::Forms::GroupBox());
			this->radioButton_loop_octave = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_loop_strict = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_loop_all = (gcnew System::Windows::Forms::RadioButton());
			this->checkBoxMaxAmbitus = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxRemoveRotations = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_m_11 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_m_10 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_m_9 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_m_8 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_m_7 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_m_6 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_m_5 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_m_4 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_m_3 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_m_2 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_m_1 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_p_11 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_p_10 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_p_9 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_p_8 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_p_7 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_p_6 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_p_5 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_p_4 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_p_3 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_interv_p_2 = (gcnew System::Windows::Forms::CheckBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->checkBox_interv_p_1 = (gcnew System::Windows::Forms::CheckBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBoxOptions->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownMaxAmbitus))->BeginInit();
			this->groupBoxLoopMode->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBoxOptions
			// 
			this->groupBoxOptions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBoxOptions->Controls->Add(this->numericUpDownMaxAmbitus);
			this->groupBoxOptions->Controls->Add(this->groupBoxLoopMode);
			this->groupBoxOptions->Controls->Add(this->checkBoxMaxAmbitus);
			this->groupBoxOptions->Controls->Add(this->checkBoxRemoveRotations);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_m_11);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_m_10);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_m_9);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_m_8);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_m_7);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_m_6);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_m_5);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_m_4);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_m_3);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_m_2);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_m_1);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_p_11);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_p_10);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_p_9);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_p_8);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_p_7);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_p_6);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_p_5);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_p_4);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_p_3);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_p_2);
			this->groupBoxOptions->Controls->Add(this->label3);
			this->groupBoxOptions->Controls->Add(this->label2);
			this->groupBoxOptions->Controls->Add(this->checkBox_interv_p_1);
			this->groupBoxOptions->Controls->Add(this->label1);
			this->groupBoxOptions->Location = System::Drawing::Point(12, 84);
			this->groupBoxOptions->Name = L"groupBoxOptions";
			this->groupBoxOptions->Size = System::Drawing::Size(577, 406);
			this->groupBoxOptions->TabIndex = 6;
			this->groupBoxOptions->TabStop = false;
			this->groupBoxOptions->Text = L"Options";
			// 
			// numericUpDownMaxAmbitus
			// 
			this->numericUpDownMaxAmbitus->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownMaxAmbitus->Location = System::Drawing::Point(439, 178);
			this->numericUpDownMaxAmbitus->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {84, 0, 0, 0});
			this->numericUpDownMaxAmbitus->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {11, 0, 0, 0});
			this->numericUpDownMaxAmbitus->Name = L"numericUpDownMaxAmbitus";
			this->numericUpDownMaxAmbitus->Size = System::Drawing::Size(61, 22);
			this->numericUpDownMaxAmbitus->TabIndex = 29;
			this->numericUpDownMaxAmbitus->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {24, 0, 0, 0});
			// 
			// groupBoxLoopMode
			// 
			this->groupBoxLoopMode->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->groupBoxLoopMode->Controls->Add(this->radioButton_loop_octave);
			this->groupBoxLoopMode->Controls->Add(this->radioButton_loop_strict);
			this->groupBoxLoopMode->Controls->Add(this->radioButton_loop_all);
			this->groupBoxLoopMode->Location = System::Drawing::Point(291, 35);
			this->groupBoxLoopMode->Name = L"groupBoxLoopMode";
			this->groupBoxLoopMode->Size = System::Drawing::Size(263, 111);
			this->groupBoxLoopMode->TabIndex = 28;
			this->groupBoxLoopMode->TabStop = false;
			this->groupBoxLoopMode->Text = L"Mode de boucle";
			// 
			// radioButton_loop_octave
			// 
			this->radioButton_loop_octave->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->radioButton_loop_octave->AutoSize = true;
			this->radioButton_loop_octave->Location = System::Drawing::Point(21, 79);
			this->radioButton_loop_octave->Name = L"radioButton_loop_octave";
			this->radioButton_loop_octave->Size = System::Drawing::Size(230, 21);
			this->radioButton_loop_octave->TabIndex = 29;
			this->radioButton_loop_octave->Text = L"Bouclables, y compris à l\'octave";
			this->radioButton_loop_octave->UseVisualStyleBackColor = true;
			// 
			// radioButton_loop_strict
			// 
			this->radioButton_loop_strict->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->radioButton_loop_strict->AutoSize = true;
			this->radioButton_loop_strict->Location = System::Drawing::Point(21, 52);
			this->radioButton_loop_strict->Name = L"radioButton_loop_strict";
			this->radioButton_loop_strict->Size = System::Drawing::Size(172, 21);
			this->radioButton_loop_strict->TabIndex = 28;
			this->radioButton_loop_strict->Text = L"Strictement bouclables";
			this->radioButton_loop_strict->UseVisualStyleBackColor = true;
			// 
			// radioButton_loop_all
			// 
			this->radioButton_loop_all->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->radioButton_loop_all->AutoSize = true;
			this->radioButton_loop_all->Checked = true;
			this->radioButton_loop_all->Location = System::Drawing::Point(21, 25);
			this->radioButton_loop_all->Name = L"radioButton_loop_all";
			this->radioButton_loop_all->Size = System::Drawing::Size(181, 21);
			this->radioButton_loop_all->TabIndex = 27;
			this->radioButton_loop_all->TabStop = true;
			this->radioButton_loop_all->Text = L"Garder toutes les séries";
			this->radioButton_loop_all->UseVisualStyleBackColor = true;
			this->radioButton_loop_all->CheckedChanged += gcnew System::EventHandler(this, &OF_SerialWithInterv::radioButton_loop_all_CheckedChanged);
			// 
			// checkBoxMaxAmbitus
			// 
			this->checkBoxMaxAmbitus->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBoxMaxAmbitus->AutoSize = true;
			this->checkBoxMaxAmbitus->Location = System::Drawing::Point(291, 179);
			this->checkBoxMaxAmbitus->Name = L"checkBoxMaxAmbitus";
			this->checkBoxMaxAmbitus->Size = System::Drawing::Size(142, 21);
			this->checkBoxMaxAmbitus->TabIndex = 26;
			this->checkBoxMaxAmbitus->Text = L"Ambitus maximal :";
			this->checkBoxMaxAmbitus->UseVisualStyleBackColor = true;
			// 
			// checkBoxRemoveRotations
			// 
			this->checkBoxRemoveRotations->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBoxRemoveRotations->AutoSize = true;
			this->checkBoxRemoveRotations->Enabled = false;
			this->checkBoxRemoveRotations->Location = System::Drawing::Point(291, 152);
			this->checkBoxRemoveRotations->Name = L"checkBoxRemoveRotations";
			this->checkBoxRemoveRotations->Size = System::Drawing::Size(255, 21);
			this->checkBoxRemoveRotations->TabIndex = 26;
			this->checkBoxRemoveRotations->Text = L"Retirer séries obtenues par rotation";
			this->checkBoxRemoveRotations->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_m_11
			// 
			this->checkBox_interv_m_11->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_m_11->AutoSize = true;
			this->checkBox_interv_m_11->Location = System::Drawing::Point(151, 351);
			this->checkBox_interv_m_11->Name = L"checkBox_interv_m_11";
			this->checkBox_interv_m_11->Size = System::Drawing::Size(120, 21);
			this->checkBox_interv_m_11->TabIndex = 24;
			this->checkBox_interv_m_11->Text = L"7ème majeure";
			this->checkBox_interv_m_11->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_m_10
			// 
			this->checkBox_interv_m_10->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_m_10->AutoSize = true;
			this->checkBox_interv_m_10->Location = System::Drawing::Point(151, 324);
			this->checkBox_interv_m_10->Name = L"checkBox_interv_m_10";
			this->checkBox_interv_m_10->Size = System::Drawing::Size(120, 21);
			this->checkBox_interv_m_10->TabIndex = 23;
			this->checkBox_interv_m_10->Text = L"7ème mineure";
			this->checkBox_interv_m_10->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_m_9
			// 
			this->checkBox_interv_m_9->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_m_9->AutoSize = true;
			this->checkBox_interv_m_9->Location = System::Drawing::Point(151, 297);
			this->checkBox_interv_m_9->Name = L"checkBox_interv_m_9";
			this->checkBox_interv_m_9->Size = System::Drawing::Size(105, 21);
			this->checkBox_interv_m_9->TabIndex = 22;
			this->checkBox_interv_m_9->Text = L"6te majeure";
			this->checkBox_interv_m_9->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_m_8
			// 
			this->checkBox_interv_m_8->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_m_8->AutoSize = true;
			this->checkBox_interv_m_8->Location = System::Drawing::Point(151, 270);
			this->checkBox_interv_m_8->Name = L"checkBox_interv_m_8";
			this->checkBox_interv_m_8->Size = System::Drawing::Size(105, 21);
			this->checkBox_interv_m_8->TabIndex = 21;
			this->checkBox_interv_m_8->Text = L"6te mineure";
			this->checkBox_interv_m_8->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_m_7
			// 
			this->checkBox_interv_m_7->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_m_7->AutoSize = true;
			this->checkBox_interv_m_7->Checked = true;
			this->checkBox_interv_m_7->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_interv_m_7->Location = System::Drawing::Point(151, 243);
			this->checkBox_interv_m_7->Name = L"checkBox_interv_m_7";
			this->checkBox_interv_m_7->Size = System::Drawing::Size(72, 21);
			this->checkBox_interv_m_7->TabIndex = 20;
			this->checkBox_interv_m_7->Text = L"Quinte";
			this->checkBox_interv_m_7->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_m_6
			// 
			this->checkBox_interv_m_6->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_m_6->AutoSize = true;
			this->checkBox_interv_m_6->Location = System::Drawing::Point(151, 216);
			this->checkBox_interv_m_6->Name = L"checkBox_interv_m_6";
			this->checkBox_interv_m_6->Size = System::Drawing::Size(67, 21);
			this->checkBox_interv_m_6->TabIndex = 19;
			this->checkBox_interv_m_6->Text = L"Triton";
			this->checkBox_interv_m_6->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_m_5
			// 
			this->checkBox_interv_m_5->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_m_5->AutoSize = true;
			this->checkBox_interv_m_5->Location = System::Drawing::Point(151, 189);
			this->checkBox_interv_m_5->Name = L"checkBox_interv_m_5";
			this->checkBox_interv_m_5->Size = System::Drawing::Size(74, 21);
			this->checkBox_interv_m_5->TabIndex = 18;
			this->checkBox_interv_m_5->Text = L"Quarte";
			this->checkBox_interv_m_5->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_m_4
			// 
			this->checkBox_interv_m_4->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_m_4->AutoSize = true;
			this->checkBox_interv_m_4->Location = System::Drawing::Point(151, 162);
			this->checkBox_interv_m_4->Name = L"checkBox_interv_m_4";
			this->checkBox_interv_m_4->Size = System::Drawing::Size(108, 21);
			this->checkBox_interv_m_4->TabIndex = 17;
			this->checkBox_interv_m_4->Text = L"3ce majeure";
			this->checkBox_interv_m_4->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_m_3
			// 
			this->checkBox_interv_m_3->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_m_3->AutoSize = true;
			this->checkBox_interv_m_3->Location = System::Drawing::Point(151, 135);
			this->checkBox_interv_m_3->Name = L"checkBox_interv_m_3";
			this->checkBox_interv_m_3->Size = System::Drawing::Size(108, 21);
			this->checkBox_interv_m_3->TabIndex = 16;
			this->checkBox_interv_m_3->Text = L"3ce mineure";
			this->checkBox_interv_m_3->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_m_2
			// 
			this->checkBox_interv_m_2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_m_2->AutoSize = true;
			this->checkBox_interv_m_2->Checked = true;
			this->checkBox_interv_m_2->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_interv_m_2->Location = System::Drawing::Point(151, 108);
			this->checkBox_interv_m_2->Name = L"checkBox_interv_m_2";
			this->checkBox_interv_m_2->Size = System::Drawing::Size(55, 21);
			this->checkBox_interv_m_2->TabIndex = 15;
			this->checkBox_interv_m_2->Text = L"Ton";
			this->checkBox_interv_m_2->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_m_1
			// 
			this->checkBox_interv_m_1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_m_1->AutoSize = true;
			this->checkBox_interv_m_1->Checked = true;
			this->checkBox_interv_m_1->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_interv_m_1->Location = System::Drawing::Point(151, 81);
			this->checkBox_interv_m_1->Name = L"checkBox_interv_m_1";
			this->checkBox_interv_m_1->Size = System::Drawing::Size(87, 21);
			this->checkBox_interv_m_1->TabIndex = 14;
			this->checkBox_interv_m_1->Text = L"Demi-ton";
			this->checkBox_interv_m_1->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_p_11
			// 
			this->checkBox_interv_p_11->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_p_11->AutoSize = true;
			this->checkBox_interv_p_11->Location = System::Drawing::Point(25, 351);
			this->checkBox_interv_p_11->Name = L"checkBox_interv_p_11";
			this->checkBox_interv_p_11->Size = System::Drawing::Size(120, 21);
			this->checkBox_interv_p_11->TabIndex = 13;
			this->checkBox_interv_p_11->Text = L"7ème majeure";
			this->checkBox_interv_p_11->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_p_10
			// 
			this->checkBox_interv_p_10->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_p_10->AutoSize = true;
			this->checkBox_interv_p_10->Location = System::Drawing::Point(25, 324);
			this->checkBox_interv_p_10->Name = L"checkBox_interv_p_10";
			this->checkBox_interv_p_10->Size = System::Drawing::Size(120, 21);
			this->checkBox_interv_p_10->TabIndex = 12;
			this->checkBox_interv_p_10->Text = L"7ème mineure";
			this->checkBox_interv_p_10->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_p_9
			// 
			this->checkBox_interv_p_9->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_p_9->AutoSize = true;
			this->checkBox_interv_p_9->Location = System::Drawing::Point(25, 297);
			this->checkBox_interv_p_9->Name = L"checkBox_interv_p_9";
			this->checkBox_interv_p_9->Size = System::Drawing::Size(105, 21);
			this->checkBox_interv_p_9->TabIndex = 11;
			this->checkBox_interv_p_9->Text = L"6te majeure";
			this->checkBox_interv_p_9->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_p_8
			// 
			this->checkBox_interv_p_8->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_p_8->AutoSize = true;
			this->checkBox_interv_p_8->Location = System::Drawing::Point(25, 270);
			this->checkBox_interv_p_8->Name = L"checkBox_interv_p_8";
			this->checkBox_interv_p_8->Size = System::Drawing::Size(105, 21);
			this->checkBox_interv_p_8->TabIndex = 10;
			this->checkBox_interv_p_8->Text = L"6te mineure";
			this->checkBox_interv_p_8->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_p_7
			// 
			this->checkBox_interv_p_7->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_p_7->AutoSize = true;
			this->checkBox_interv_p_7->Checked = true;
			this->checkBox_interv_p_7->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_interv_p_7->Location = System::Drawing::Point(25, 243);
			this->checkBox_interv_p_7->Name = L"checkBox_interv_p_7";
			this->checkBox_interv_p_7->Size = System::Drawing::Size(72, 21);
			this->checkBox_interv_p_7->TabIndex = 9;
			this->checkBox_interv_p_7->Text = L"Quinte";
			this->checkBox_interv_p_7->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_p_6
			// 
			this->checkBox_interv_p_6->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_p_6->AutoSize = true;
			this->checkBox_interv_p_6->Location = System::Drawing::Point(25, 216);
			this->checkBox_interv_p_6->Name = L"checkBox_interv_p_6";
			this->checkBox_interv_p_6->Size = System::Drawing::Size(67, 21);
			this->checkBox_interv_p_6->TabIndex = 8;
			this->checkBox_interv_p_6->Text = L"Triton";
			this->checkBox_interv_p_6->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_p_5
			// 
			this->checkBox_interv_p_5->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_p_5->AutoSize = true;
			this->checkBox_interv_p_5->Location = System::Drawing::Point(25, 189);
			this->checkBox_interv_p_5->Name = L"checkBox_interv_p_5";
			this->checkBox_interv_p_5->Size = System::Drawing::Size(74, 21);
			this->checkBox_interv_p_5->TabIndex = 7;
			this->checkBox_interv_p_5->Text = L"Quarte";
			this->checkBox_interv_p_5->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_p_4
			// 
			this->checkBox_interv_p_4->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_p_4->AutoSize = true;
			this->checkBox_interv_p_4->Location = System::Drawing::Point(25, 162);
			this->checkBox_interv_p_4->Name = L"checkBox_interv_p_4";
			this->checkBox_interv_p_4->Size = System::Drawing::Size(108, 21);
			this->checkBox_interv_p_4->TabIndex = 6;
			this->checkBox_interv_p_4->Text = L"3ce majeure";
			this->checkBox_interv_p_4->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_p_3
			// 
			this->checkBox_interv_p_3->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_p_3->AutoSize = true;
			this->checkBox_interv_p_3->Location = System::Drawing::Point(25, 135);
			this->checkBox_interv_p_3->Name = L"checkBox_interv_p_3";
			this->checkBox_interv_p_3->Size = System::Drawing::Size(108, 21);
			this->checkBox_interv_p_3->TabIndex = 5;
			this->checkBox_interv_p_3->Text = L"3ce mineure";
			this->checkBox_interv_p_3->UseVisualStyleBackColor = true;
			// 
			// checkBox_interv_p_2
			// 
			this->checkBox_interv_p_2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_p_2->AutoSize = true;
			this->checkBox_interv_p_2->Checked = true;
			this->checkBox_interv_p_2->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_interv_p_2->Location = System::Drawing::Point(25, 108);
			this->checkBox_interv_p_2->Name = L"checkBox_interv_p_2";
			this->checkBox_interv_p_2->Size = System::Drawing::Size(55, 21);
			this->checkBox_interv_p_2->TabIndex = 4;
			this->checkBox_interv_p_2->Text = L"Ton";
			this->checkBox_interv_p_2->UseVisualStyleBackColor = true;
			// 
			// label3
			// 
			this->label3->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(148, 61);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(91, 17);
			this->label3->TabIndex = 3;
			this->label3->Text = L"Descendants";
			// 
			// label2
			// 
			this->label2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(22, 61);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(82, 17);
			this->label2->TabIndex = 2;
			this->label2->Text = L"Ascendants";
			// 
			// checkBox_interv_p_1
			// 
			this->checkBox_interv_p_1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->checkBox_interv_p_1->AutoSize = true;
			this->checkBox_interv_p_1->Checked = true;
			this->checkBox_interv_p_1->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_interv_p_1->Location = System::Drawing::Point(25, 81);
			this->checkBox_interv_p_1->Name = L"checkBox_interv_p_1";
			this->checkBox_interv_p_1->Size = System::Drawing::Size(87, 21);
			this->checkBox_interv_p_1->TabIndex = 1;
			this->checkBox_interv_p_1->Text = L"Demi-ton";
			this->checkBox_interv_p_1->UseVisualStyleBackColor = true;
			// 
			// label1
			// 
			this->label1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(22, 35);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(142, 17);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Intervalles autorisés :";
			// 
			// OF_SerialWithInterv
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(601, 540);
			this->Controls->Add(this->groupBoxOptions);
			this->Name = L"OF_SerialWithInterv";
			this->Text = L"OF_SerialWithInterv";
			this->Controls->SetChildIndex(this->groupBoxOptions, 0);
			this->groupBoxOptions->ResumeLayout(false);
			this->groupBoxOptions->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownMaxAmbitus))->EndInit();
			this->groupBoxLoopMode->ResumeLayout(false);
			this->groupBoxLoopMode->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	
	};
}
