#include <string>
#include <math.h>
#include "camera.h" 

#include "ImGUI/imgui_sdl.h"
#include "ImGUI/imgui.h"

int aaa = 0;

bool insideoutsidetest(Triangle triangle, vec3 &P)
{
    
    vec3 p1 = triangle.vertex[0].pos;
    vec3 p2 = triangle.vertex[1].pos;
    vec3 p3 = triangle.vertex[2].pos;

    vec3 normTriag = (cross(p2-p1, p3-p1));  
    
    vec3 cOut1 = -cross(P - p1, p2 - p1);
    vec3 cOut2 = -cross(P - p2, p3 - p2);
    vec3 cOut3 = -cross(P - p3, p1 - p3);

    if((dot(cOut1, normTriag) > 0 && dot(cOut2, normTriag) > 0) && dot(cOut3, normTriag) > 0)
    {
		// printf("true\n");
        return true;
    }
	// printf("false\n");
    return false;
}

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {

		SDL_Window* window = SDL_CreateWindow("Projeto PG - Grupo 3", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
		SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

		if (window && renderer) {
			vec3 Pos = vec3 (0,0,5);
			vec3 At = vec3 (0,0,-1);
			SDL_bool done = SDL_FALSE;
			SDL_SetRelativeMouseMode(SDL_FALSE);

			std::vector<Obj> objects;
			objects.push_back(Obj("monkey_smooth.obj"));

			ImGui::CreateContext();
			ImGuiSDL::Initialize(renderer, WIDTH, HEIGHT);

			camera cam(Pos, At, vec3(0, 1, 0), 90.0f, 1.f, WIDTH, HEIGHT, 1000);

			float my_color[4];
			bool my_tool_active;

			while (!done) {
				SDL_Event event;
				ImGuiIO& io = ImGui::GetIO();
				std::clock_t then = std::clock();

				int mouseX, mouseY;
				const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

				io.DeltaTime = 1.0f / 60.0f;
				io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
				io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
				io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

				ImGui::NewFrame();

				// Create a window called "My First Tool", with a menu bar.
				ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("File"))
					{
						if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
						if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
						if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}

				// Edit a color (stored as ~4 floats)
				ImGui::ColorEdit4("Color", my_color);
				// Plot some values
				const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
				ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

				// Display contents in a scrolling region
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Pojeto di PeGue");
				ImGui::BeginChild("Scrolling");
				ImGui::Text("Vo mata o Krushtu\n");
				ImGui::EndChild();
				ImGui::End();

				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
				SDL_RenderClear(renderer); // clear previous frame generated image

				cam.render_scene(objects, renderer); // feed triangle data onto renderer

				ImGui::Render();
				ImGuiSDL::Render(ImGui::GetDrawData());

				SDL_RenderPresent(renderer); // present the generated triangle data onto screen
				
				while (SDL_PollEvent(&event)) {

					if (event.type == SDL_KEYDOWN) {
						if (event.key.keysym.sym == SDLK_a) {

							matrix44(			
							cos(1.0 * 3.14159265/ 180.0), 0, sin(1.0 * 3.14159265/ 180.0)*(-1) , 0.0,
							0, 1, 0, 0.0,
							sin(1.0 * 3.14159265/ 180.0), 0, cos(1.0 * 3.14159265/ 180.0), 0.0,
							0, 0, 0, 1.0
							).mult_vec_matrix(Pos, Pos);

							//At += cam.axisX; 
							printf("A %f,%f,%f\n", Pos.x(), Pos.y(), Pos.z());
							cam.look_at(Pos, At, vec3(0, 1, 0));
						}
					}
					if (event.type == SDL_KEYDOWN) {
						if (event.key.keysym.sym == SDLK_w) {
							Pos -= (cam.axisZ)/10;
							//At -= cam.axisZ;
							printf("W %f,%f,%f\n", Pos.x(), Pos.y(), Pos.z());
							cam.look_at(Pos,At, vec3(0, 1, 0));
						}
					}
					if (event.type == SDL_KEYDOWN) {
						if (event.key.keysym.sym == SDLK_s) {
							Pos += (cam.axisZ)/10;
							//At += cam.axisZ;
							printf("S %f,%f,%f\n", Pos.x(), Pos.y(), Pos.z());
							cam.look_at(Pos,At, vec3(0, 1, 0));
						}
					}
					if (event.type == SDL_KEYDOWN) {
						if (event.key.keysym.sym == SDLK_d) {

							matrix44(			
							cos(1.0 * 3.14159265/ 180.0), 0, sin(1.0 * 3.14159265/ 180.0) , 0.0,
							0, 1, 0, 0.0,
							sin(1.0 * 3.14159265/ 180.0)*(-1), 0, cos(1.0 * 3.14159265/ 180.0), 0.0,
							0, 0, 0, 1.0
							).mult_vec_matrix(Pos, Pos);

							//At -= cam.axisX;
							printf("D %f,%f,%f\n", Pos.x(), Pos.y(), Pos.z());
							cam.look_at(Pos,At, vec3(0, 1, 0));
						}
					}


					// mouse pressed event
					if (event.type == SDL_MOUSEBUTTONDOWN)
					{
						//If the left mouse button was pressed
						if (event.button.button == SDL_BUTTON_RIGHT)
						{
						}
						if (event.button.button == SDL_BUTTON_LEFT)
						{
							bool res = false;
							//Relative mouse mode moves mouse to center of window
							//every frame. Useful if you want to use mouse and not worry
							//about it going off screen

							int x, y;
							SDL_GetMouseState(&x, &y);
							
							//SDL_SetRelativeMouseMode(SDL_TRUE);
							float Px = -(2 * ((float)x/(float)WIDTH) - 1) * cam.right/cam._near;
							float Py = (1 - 2*((float)y/(float)HEIGHT)) * cam.top/cam._near;
							vec3 RayDirection = vec3(Px, Py, -1);
							
							vec3 RDWorld;

							cam.camToWorld.mult_vec_matrix(RayDirection, RDWorld);
							
							RDWorld.make_unit_vector();

							for (auto object : objects) 
							{ // Se não rodar com object, trocar por obj
								for (int i = 0; i < object.mesh.tris.size(); i++)
								{


									vec3 p1 = object.mesh.tris[i].vertex[0].pos;
									vec3 p2 = object.mesh.tris[i].vertex[1].pos;
									vec3 p3 = object.mesh.tris[i].vertex[2].pos;

									vec3 normTriag = (cross(p2-p1, p3-p1));  

									float D = dot(normTriag, p1);

									float t = -(dot(normTriag, cam._from) - D)/dot(normTriag, RDWorld);

									if(dot(normTriag, RDWorld) >= 0.00001 && t >= 0)
									{
										vec3 pInter;
										pInter[0] = cam._from[0] + (t*RDWorld[0]);
										pInter[1] = cam._from[1] + (t*RDWorld[1]);
										pInter[2] = cam._from[2] + (t*RDWorld[2]);	
										
										res = insideoutsidetest(object.mesh.tris[i], pInter);
										
									}
									if(res)
									{
										aaa++;
										printf("clicou %d\n",aaa);

									}
								}
							
							}
						
						}
					}
					if (event.type == SDL_MOUSEBUTTONUP)
					{
						//If the left mouse button was released
						if (event.button.button == SDL_BUTTON_LEFT)
						{
							SDL_SetRelativeMouseMode(SDL_FALSE);
						}
					}

					// mouse move event
					if (event.type == SDL_MOUSEMOTION)
					{
						//get relative mouse motion, i.e mouse delta x,y from last frame
						float x = event.motion.xrel;
						float y = event.motion.yrel;

					}

					if (event.type == SDL_QUIT)
						done = SDL_TRUE;

				}
			}
		}

		if (renderer) {
			SDL_DestroyRenderer(renderer);
		}
		if (window) {
			SDL_DestroyWindow(window);
		}
	}
	SDL_Quit();
	return 0;
}
