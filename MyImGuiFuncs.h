static void ShowOverlayFPS(bool* p_open, float* fps)
{
	const float DISTANCE = 10.0f;
	static int corner = 0;
	ImGuiIO& io = ImGui::GetIO();
	if (corner != -1)
	{
		ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	}
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	if (ImGui::Begin("FPS Count", p_open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
	{
		ImGui::Text("FPS: %.1f", *fps);
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
			if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			if (p_open && ImGui::MenuItem("Close")) *p_open = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

static void ShowOverlayMatrice(bool* p_open, float* matrice, const char* matriceName)
{
	const float DISTANCE = 10.0f;
	static int corner = 1;
	ImGuiIO& io = ImGui::GetIO();
	if (corner != -1)
	{
		ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	}
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	if (ImGui::Begin("Matrice overlay", p_open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
	{
		ImGui::Text("Matrice type: %s", matriceName);
		ImGui::Text("%.1f %.1f %.1f %.1f", matrice[0], matrice[1], matrice[2], matrice[3]);
		ImGui::Text("%.1f %.1f %.1f %.1f", matrice[4], matrice[5], matrice[6], matrice[7]);
		ImGui::Text("%.1f %.1f %.1f %.1f", matrice[8], matrice[9], matrice[10], matrice[11]);
		ImGui::Text("%.1f %.1f %.1f %.1f", matrice[12], matrice[13], matrice[14], matrice[15]);
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
			if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			if (p_open && ImGui::MenuItem("Close")) *p_open = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

static void ShowWindowTree(std::vector<PointLight*>* lights, float* gamma, float* exposure, int* blur, int* blurWeightNum, float* blurA) {
	if (ImGui::CollapsingHeader("Lightning")){
		if (ImGui::TreeNode("Point lights"))
		{
			for (int i = 0; i < lights->size(); i++)
				if (ImGui::TreeNode((void*)(intptr_t)i, "Light %d", i))
				{
					ImGui::Text("Light attenuation");
					ImGui::DragFloat("Constant", &lights->at(i)->constant, 0.001f, 0.0f, 0.0f, "%.06f");
					ImGui::DragFloat("Linear", &lights->at(i)->linear, 0.0001f, 0.0f, 0.0f, "%.06f");
					ImGui::DragFloat("Quadratic", &lights->at(i)->quadratic, 0.00001f, 0.0f, 0.0f, "%.06f");
					ImGui::Text("Color");
					ImGui::ColorEdit3("RGB", glm::value_ptr(lights->at(i)->color));
					ImGui::Text("Position");
					ImGui::DragFloat("X", &lights->at(i)->position.x, 0.05f);
					ImGui::DragFloat("Y", &lights->at(i)->position.y, 0.05f);
					ImGui::DragFloat("Z", &lights->at(i)->position.z, 0.05f);
					ImGui::TreePop();
				}
			ImGui::TreePop();
		}
	}
	if (ImGui::CollapsingHeader("Screen")){
		if (ImGui::TreeNode("HDR"))
		{
			ImGui::DragFloat("Gamma", gamma, 0.1f, 0.0f, 0.0f, "%.01f");
			ImGui::DragFloat("Exposure", exposure, 0.01f, 0.0f, 0.0f, "%.01f");
			ImGui::TreePop();
		}
	}
	if (ImGui::CollapsingHeader("VFX")){
		if (ImGui::TreeNode("Blur"))
		{
			ImGui::DragInt("Blur Weights Number", blurWeightNum, 1.0f, 0, 32);
			ImGui::DragFloat("Blur Funcstion A Constant", blurA, 0.01f, 0.0f, 0.0f, "%.01f");
			ImGui::DragInt("Blur Cycles", blur, 1.0f, 0, 32);
			ImGui::TreePop();
		}
	}
}