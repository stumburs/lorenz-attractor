#include <raylib.h>
#include <iostream>
#include <vector>
#include <raymath.h>

struct Point
{
	Vector3 pos;
	Color color;
};

Vector3 attractor_center(const std::vector<Point>& points)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	for (const auto& point : points)
	{
		x += point.pos.x;
		y += point.pos.y;
		z += point.pos.z;
	}
	return { x / points.size(), y / points.size(), z / points.size() };
}

Camera3D camera;

std::vector<Point> points;

Vector3 last_point = { 0.0f, 0.0f, 0.0f };

//--------------------------------------------------------------
// User modifiable variables
//--------------------------------------------------------------
int window_width = 1920;
int window_height = 1080;
int max_fps = 0;

// Starting values
float x = 5.0f;
float y = 5.0f;
float z = 5.0f;

// Lorenz values
// https://en.wikipedia.org/wiki/Lorenz_system#Analysis
const float a = 10.0f;				// sigma
const float b = 28.0f;				// rho
const float c = 8.0f / 3.0f;		// beta

// Calculation time step
// Larger numbers = faster, but less accurate
const float time_step = 0.001f;

// Shape scaling factor
const float scale = 3.0;

// Starting hue value
float hue = 0.0f;
float hue_change_per_frame = 0.002f;

std::size_t max_point_amount = 20000;

int main()
{
	SetConfigFlags(ConfigFlags::FLAG_WINDOW_UNDECORATED);
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(window_width, window_height, "lorenz attractor");
	SetTargetFPS(max_fps);

	camera.fovy = 90.0f;
	camera.position = { 100.0f, 50.0f, 0.0f };
	camera.projection = CameraProjection::CAMERA_PERSPECTIVE;
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.target = { 0.0f, 0.0f, 80.0f };

	while (!WindowShouldClose())
	{
		// Orbital camera automatically rotates
		UpdateCamera(&camera, CAMERA_ORBITAL);
		// Update
		{
			double dx = a * (y - x);
			double dy = x * (b - z) - y;
			double dz = x * y - c * z;

			x = x + dx * time_step;
			y = y + dy * time_step;
			z = z + dz * time_step;

			Color color = ColorFromHSV(hue, 1.0f, 1.0f);
			hue += hue_change_per_frame;
			if (hue > 360.0f)
				hue = 0.0f;

			Point point{ { x * scale, y * scale, z * scale }, color };
			points.push_back(point);

			if (points.size() > max_point_amount)
				points.erase(points.begin());

			// Set camera target to average position of all points
			camera.target = attractor_center(points);
		}

		// Draw
		BeginDrawing();
		{
			ClearBackground(BLACK);
			BeginMode3D(camera);
			{
				for (auto& point : points)
				{
					DrawLine3D(last_point, { point.pos.x, point.pos.y, point.pos.z }, point.color);
					last_point = { point.pos.x, point.pos.y, point.pos.z };
				}
				last_point = points.begin()->pos;
			}
			EndMode3D();
			DrawText(TextFormat("Points: %d", points.size()), 20, 40, 20, GREEN);
			DrawFPS(20, 20);

		}
		EndDrawing();
	}
	CloseWindow();
}
