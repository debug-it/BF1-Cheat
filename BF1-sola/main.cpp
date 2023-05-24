#include "includes.h"
#include <thread>


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;
ImVec2 ToImVec2(Vec2 vec)
{
	return ImVec2(vec.x, vec.y);
}

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}
ClientSoldierEntity* GetClosestPlayerToCenterScreen()
{
	// Get the center of the screen
	float centerX = ImGui::GetIO().DisplaySize.x * 0.5f;
	float centerY = ImGui::GetIO().DisplaySize.y * 0.5f;

	// Define variables to track the closest player
	float closestDistance = 500; // Initialize with a large value
	ClientSoldierEntity* closestPlayer = nullptr;

	for (int i = 0; i <= 64; i++)
	{
		// ...
		ClientPlayer* local_player = GetLocalPlayer();
		if (!IsValidPtr(local_player)) return nullptr;

		/* World to screen the players position */
		Vec2 base_screen;
		ClientPlayer* player = GetPlayerById(i);

		/* Check that the player is valid */
		if (!IsValidPtr(player)) continue;

		bool team = false; if (player->teamId == local_player->teamId) team = true;

		if (!team)
		{
			/* Get this iterations soldier */
			ClientSoldierEntity* soldier = player->clientSoldierEntity;
			if (!IsValidPtr(soldier)) continue;
			if (!W2S(soldier->location, base_screen)) continue;

			/* Calculate the distance from the center of the screen */
			float distance = std::sqrt((base_screen.x - centerX) * (base_screen.x - centerX) + (base_screen.y - centerY) * (base_screen.y - centerY));

			/* Update the closest player if the current player is closer */
			if (distance < closestDistance)
			{
				closestDistance = distance;
				closestPlayer = soldier;
			}
		}
		

		// ...
	}

	return closestPlayer;
}


void DrawHealthBar(ImVec2 startPos, ImVec2 endPos, float health, float maxHealth) {
	ImU32 greenColor = IM_COL32(46, 204, 113, 255);    // Green color (e.g., RGB: 46, 204, 113)
	ImU32 redColor = IM_COL32(229, 57, 53, 255); // Red color (e.g., RGB: 229, 57, 53)

	// Calculate the width of the health bar based on the health percentage
	float barWidth = (endPos.x - startPos.x) * (health / maxHealth);
	ImVec2 barEnd = ImVec2(startPos.x + barWidth, startPos.y);

	// Draw red line as the background
	ImGui::GetOverlayDrawList()->AddLine(startPos, endPos, redColor, 2.0f);

	// Draw green overlay indicating remaining health
	ImGui::GetOverlayDrawList()->AddLine(startPos, barEnd, greenColor, 2.0f);
}

int hackloop()
{
	/* Attempt to get the local player instance */

	ClientPlayer* local_player = GetLocalPlayer();

	if (!IsValidPtr(local_player)) return 0;



	std::string name = local_player->name;
	ClientSoldierEntity* local_soldier = local_player->clientSoldierEntity;
	if (!IsValidPtr(local_soldier)) return 0;

	/* Walk the entirety of the player list */
	for (int i = 0; i <= 64; i++)
	{
		/* Get this iterations player */
		ClientPlayer* player = GetPlayerById(i);

		/* Check that the player is valid */
		if (!IsValidPtr(player)) continue;
	
		bool team = false; if (player->teamId == local_player->teamId) team = true;

		/* Get this iterations soldier */
		ClientSoldierEntity* soldier = player->clientSoldierEntity;

		/* Check that Client soldier is valid */
		if (!IsValidPtr(soldier)) continue;

		/* Check that this player is valid */
		if (soldier->IsDead()) continue;

		/* World to screen the players position */
		Vec2 base_screen; if (!W2S(soldier->location, base_screen)) continue;

		/* World to screen the players head positon */
		Vec2 head_screen; if (!W2S(soldier->location + Vec3(soldier->GetAABB().max.x, soldier->GetAABB().max.y, soldier->GetAABB().max.z), head_screen)) continue;

	
		Vec3 location = soldier->location;
		Vec2 c1; Vec2 c2;
		W2S(soldier->location + Vec3(soldier->GetAABB().min.x, soldier->GetAABB().min.y, soldier->GetAABB().max.z), c1);
		W2S(soldier->location + Vec3(soldier->GetAABB().max.x, soldier->GetAABB().max.y, soldier->GetAABB().max.z), c2);
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();


		if (Utils::Cheat::boxesp) {
			

			//std::cout << c2.x << "   " << c2.y << std::endl;
			ImColor boxColor;
			if (!team) {
				boxColor = ImColor(1.0f, 1.0f, 1.0f);
				if (Utils::Cheat::vischeck && soldier->IsVisible())
				{
					drawList->AddRect(ImVec2(c1.x, c1.y), ImVec2(c2.x, c2.y), IM_COL32(52, 235, 64, 255)); // Green outline color

				}
				else
				{
					drawList->AddRect(ImVec2(c1.x, c1.y), ImVec2(c2.x, c2.y), IM_COL32(255, 0, 0, 255)); // Red outline color

				}
			}
			else boxColor = ImColor(0.0f, 1.0f, 1.0f);

			//if (!soldier->occluded) boxColor = global->c_e_visuals_box_visible;

		
		}
		if (Utils::Cheat::healthesp)
		{
			if (!team) {

				// Draw the full health line
				float healthPercentage = (float)soldier->healthcomponent->m_Health / (float)soldier->healthcomponent->m_MaxHealth;

				// Calculate the height of the health indicator lines
				float fullHealthLineHeight = c2.y - c1.y;
				float remainingHealthLineHeight = fullHealthLineHeight * healthPercentage;

				ImVec2 fullHealthLineStart(c2.x - 3, c1.y);
				ImVec2 fullHealthLineEnd(c2.x - 3, c2.y);
				drawList->AddLine(fullHealthLineStart, fullHealthLineEnd, IM_COL32(255, 0, 0, 255)); // Red line color

				// Draw the remaining health line
				ImVec2 remainingHealthLineStart(c2.x - 3, c2.y);
				ImVec2 remainingHealthLineEnd(c2.x - 3, c2.y - remainingHealthLineHeight);
				drawList->AddLine(remainingHealthLineStart, remainingHealthLineEnd, IM_COL32(52, 235, 64, 255)); // Green line color
			}
			

			
		}
		if (Utils::Cheat::infoesp)
		{
			if (!team) {

				// Form the information to display
				char upper_buffer[128];
				char lower_buffer[128];

				// Create the upper text
				sprintf(upper_buffer, "[%dHP %dM]", (int)soldier->healthcomponent->m_Health, (int)CalculateDistance(local_soldier->location, soldier->location));

				// Create the lower text
				sprintf(lower_buffer, "[%s]", player->name);

				// Calculate text positions
				ImVec2 upper_text_pos = ToImVec2(head_screen);
				upper_text_pos.x -= ImGui::CalcTextSize(upper_buffer).x / 2;

				ImVec2 lower_text_pos = ToImVec2(base_screen);
				lower_text_pos.x -= ImGui::CalcTextSize(lower_buffer).x / 2;

				// Draw upper text

				ImGui::GetOverlayDrawList()->AddText(upper_text_pos, IM_COL32(176, 68, 219, 255), upper_buffer);

				// Draw lower text
				ImGui::GetOverlayDrawList()->AddText(lower_text_pos, IM_COL32(176, 68, 219, 255), lower_buffer);
			}
		}

		if (Utils::Cheat::aimbot && GetAsyncKeyState(VK_ADD) & 0x8000)
		{
			Vec2 center = Vec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
			/* Create vars to keep track of current target and target info */
			float closestdist = 999.f;
			ClientSoldierEntity* closest = GetClosestPlayerToCenterScreen();


			if (closest != nullptr) {
				/* Redo checks incase somehow in the time it takes to get here something changes */

				/* Check that soldier is valid */
				if (!IsValidPtr(closest)) continue;
				/* Check that this player is valid */
				if (closest->IsDead()) continue;
				/* Check that the player is visible */
				if (closest->IsVisible()) continue;
				/* Check if player isnt occluded */
				if (!closest->occluded) continue;

				/* Do another head W2S, calculate differences, and move mouse */
				Vec2 head_screen; if (!W2S(closest->location + Vec3(closest->GetAABB().max.x, closest->GetAABB().max.y, closest->GetAABB().max.z), head_screen)) continue;
				DWORD movex = center.x - head_screen.x;
				DWORD movey = center.y - head_screen.y;
				//std::string debug = "{ X: " + std::to_string(center.x) + " - " + std::to_string(head_screen.x) + ", Y: " + std::to_string(center.y) + " - " + std::to_string(head_screen.y) + " }";
				//std::string debug = "{ X: " + std::to_string((int)movex) + ", Y: " + std::to_string((int)movey) + " }";
				//Log::Debug(debug);
				//if ((int)movex != 0)
					//movex < 0 ? movex = -1 : movex = 1;
				//if ((int)movey != 0)
					//movey < 0 ? movey = -1 : movey = 1;
			
			
			}
		}
		if (Utils::Cheat::skeletonesp) {
			/* draw skeleton */
			if (!team) {

				bool vis = soldier->IsVisible();
				DrawSkeleton(soldier, ImVec4(0, 1, 0, 1), vis);

			}
		}

	}
	return 0;
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{

	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}
	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Utils::Cheat::menuCount = 500;
	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

	ImGui::Begin("BF1 Cheat - Bar");
	if (ImGui::BeginTabBar("Tabs"))
	{
		if (ImGui::BeginTabItem("Player"))
		{
			// Add the 4 checkboxes and 1 float slider
			ImGui::Checkbox("Info Esp", &Utils::Cheat::infoesp);
			ImGui::Checkbox("Health Esp", &Utils::Cheat::healthesp);
			ImGui::Checkbox("Box Esp", &Utils::Cheat::boxesp);
			ImGui::Checkbox("Vischeck", &Utils::Cheat::vischeck);
			ImGui::Checkbox("Skeleton Esp", &Utils::Cheat::skeletonesp);

			ImGui::Checkbox("Aimbot", &Utils::Cheat::aimbot);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::Text("You silly little cheater.", IMGUI_VERSION);
	hackloop();
	ImGui::End();

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	

	return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	AllocConsole();
	FILE* file = nullptr;
	freopen_s(&file, "CONOUT$", "w", stdout);
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	srand(time(NULL));

	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)& oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}