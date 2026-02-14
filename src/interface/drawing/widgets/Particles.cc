#include "widgets.hh"
#include <cmath>
#include <cstdlib>
#include <vector>

namespace widgets
{

    struct Particle
    {
        float x, y; // Position
        float vx, vy; // Velocity
        float rot;
        float rot_speed;
        float w, h;
        float life;
        float max_life;
        ImU32 color;
        int shape; // 0 = rect, 1 = circle, 2 = triangle
    };

    static std::vector<Particle> particles_t;
    static unsigned int particle_seed_t = 12345u;

    static float ParticleRand()
    {
        particle_seed_t ^= particle_seed_t << 13;
        particle_seed_t ^= particle_seed_t >> 17;
        particle_seed_t ^= particle_seed_t << 5;
        return (float)(particle_seed_t & 0xFFFFu) / 65535.0f;
    }

    static float ParticleRandRange(float lo, float hi)
    {
        return lo + ParticleRand() * (hi - lo);
    }

    static ImU32 ConfettiColors[] = {
        IM_COL32(255, 100, 100, 255), // Red
        IM_COL32(100, 200, 255, 255), // Blue
        IM_COL32(100, 255, 120, 255), // Green
        IM_COL32(255, 220, 80, 255),  // Yellow
        IM_COL32(200, 120, 255, 255), // Purple
        IM_COL32(255, 160, 80, 255),  // Orange
        IM_COL32(255, 130, 200, 255), // Pink
        IM_COL32(120, 220, 220, 255), // Cyan
    };
    static const int ConfettiColorCount = 8;

    void SpawnConfetti(float cx, float cy, int count)
    {
        particle_seed_t ^= (unsigned int)(cx * 100.0f) ^ (unsigned int)(cy * 73.0f) ^ (unsigned int)ImGui::GetFrameCount();

        for (int i = 0; i < count; i++)
        {
            Particle p;
            float angle = ParticleRandRange(0.0f, 6.2831853f);
            float speed = ParticleRandRange(80.0f, 320.0f) * dpi_scale_t;

            p.x = cx + ParticleRandRange(-4.0f, 4.0f) * dpi_scale_t;
            p.y = cy + ParticleRandRange(-4.0f, 4.0f) * dpi_scale_t;
            p.vx = cosf(angle) * speed;
            p.vy = sinf(angle) * speed - ParticleRandRange(40.0f, 120.0f) * dpi_scale_t;
            p.rot = ParticleRandRange(0.0f, 6.2831853f);
            p.rot_speed = ParticleRandRange(-8.0f, 8.0f);
            p.w = ParticleRandRange(3.0f, 7.0f) * dpi_scale_t;
            p.h = ParticleRandRange(2.0f, 5.0f) * dpi_scale_t;
            p.life = ParticleRandRange(0.8f, 2.0f);
            p.max_life = p.life;
            p.color = ConfettiColors[(int)(ParticleRand() * ConfettiColorCount) % ConfettiColorCount];
            p.shape = (int)(ParticleRand() * 3.0f) % 3;

            particles_t.push_back(p);
        }
    }

    void UpdateAndDrawParticles(ImDrawList* draw)
    {
        if (particles_t.empty())
            return;

        float dt = ImGui::GetIO().DeltaTime;
        if (dt <= 0.0f || dt > 0.1f) dt = 0.016f;

        float gravity = 350.0f * dpi_scale_t;
        float drag = 0.98f;

        for (int i = (int)particles_t.size() - 1; i >= 0; i--)
        {
            Particle& p = particles_t[i];

            p.vy += gravity * dt;
            p.vx *= drag;
            p.vy *= drag;
            p.x += p.vx * dt;
            p.y += p.vy * dt;
            p.rot += p.rot_speed * dt;
            p.life -= dt;

            if (p.life <= 0.0f)
            {
                particles_t.erase(particles_t.begin() + i);
                continue;
            }

            float alpha = 1.0f;
            float life_frac = p.life / p.max_life;
            if (life_frac < 0.3f)
                alpha = life_frac / 0.3f;

            int r = (p.color >> 0) & 0xFF;
            int g = (p.color >> 8) & 0xFF;
            int b = (p.color >> 16) & 0xFF;
            ImU32 col = IM_COL32(r, g, b, (int)(alpha * 255));


            float c = cosf(p.rot);
            float s = sinf(p.rot);
            float hw = p.w * 0.5f;
            float hh = p.h * 0.5f;

            if (p.shape == 0)
            {
                ImVec2 corners[4] = {
                    ImVec2(p.x + (-hw * c - (-hh) * s), p.y + (-hw * s + (-hh) * c)),
                    ImVec2(p.x + (hw * c - (-hh) * s), p.y + (hw * s + (-hh) * c)),
                    ImVec2(p.x + (hw * c - hh * s), p.y + (hw * s + hh * c)),
                    ImVec2(p.x + (-hw * c - hh * s), p.y + (-hw * s + hh * c)),
                };
                draw->AddQuadFilled(corners[0], corners[1], corners[2], corners[3], col);
            }
            else if (p.shape == 1)
            {
                draw->AddCircleFilled(ImVec2(p.x, p.y), hw, col, 6);
            }
            else
            {
                ImVec2 t1(p.x + (-hw * c - (-hh) * s), p.y + (-hw * s + (-hh) * c));
                ImVec2 t2(p.x + (hw * c - (-hh) * s), p.y + (hw * s + (-hh) * c));
                ImVec2 t3(p.x + (0 * c - hh * s), p.y + (0 * s + hh * c));
                draw->AddTriangleFilled(t1, t2, t3, col);
            }
        }
    }
}
