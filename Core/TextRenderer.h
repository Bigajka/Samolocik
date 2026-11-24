#pragma once
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <array>
#include <cctype>

class TextRenderer
{
public:
    TextRenderer()
    {
        InitShader();
        InitQuad();
    }

    ~TextRenderer()
    {
        if (VBO != 0) glDeleteBuffers(1, &VBO);
        if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (shaderID != 0) glDeleteProgram(shaderID);
    }

    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;

    void Begin(int screenWidth, int screenHeight) const
    {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(shaderID);
        glm::mat4 proj = glm::ortho(0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight), 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "Projection"), 1, GL_FALSE, glm::value_ptr(proj));
        glBindVertexArray(VAO);
    }

    void End() const
    {
        glBindVertexArray(0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

    void DrawRect(float x, float y, float w, float h, const glm::vec4& color) const
    {
        glUniform4f(glGetUniformLocation(shaderID, "uRect"), x, y, w, h);
        glUniform4fv(glGetUniformLocation(shaderID, "uColor"), 1, glm::value_ptr(color));
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // scale = rozmiar pojedynczego "piksela" litery (np. 4.0f dla HUD, 22.0f dla napisu WIN)
    void DrawText(const std::string& text, float x, float y, float scale, const glm::vec3& color) const
    {
        float cursorX = x;
        const float charAdvance = 6.0f * scale; // 5 px szerokości znaku + 1 px odstępu

        for (char rawC : text)
        {
            char c = static_cast<char>(std::toupper(static_cast<unsigned char>(rawC)));
            const auto& glyph = GetGlyph(c);

            for (int row = 0; row < 7; ++row)
            {
                for (int col = 0; col < 5; ++col)
                {
                    if (glyph[row] & (1 << (4 - col)))
                    {
                        DrawRect(
                            cursorX + col * scale,
                            y + row * scale,
                            scale,
                            scale,
                            glm::vec4(color, 1.0f)
                        );
                    }
                }
            }
            cursorX += charAdvance;
        }
    }

    float GetTextWidth(const std::string& text, float scale) const
    {
        if (text.empty()) return 0.0f;
        return (text.size() * 6.0f - 1.0f) * scale;
    }

    float GetTextHeight(float scale) const
    {
        return 7.0f * scale;
    }

private:
    GLuint shaderID = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;

    void InitShader()
    {
        const char* vertSrc = R"(
            #version 330 core
            layout (location = 0) in vec2 aPos;
            uniform mat4 Projection;
            uniform vec4 uRect;
            void main()
            {
                vec2 pos = uRect.xy + aPos * uRect.zw;
                gl_Position = Projection * vec4(pos, 0.0, 1.0);
            }
        )";

        const char* fragSrc = R"(
            #version 330 core
            out vec4 FragColor;
            uniform vec4 uColor;
            void main()
            {
                FragColor = uColor;
            }
        )";

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertSrc, nullptr);
        glCompileShader(vs);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fragSrc, nullptr);
        glCompileShader(fs);

        shaderID = glCreateProgram();
        glAttachShader(shaderID, vs);
        glAttachShader(shaderID, fs);
        glLinkProgram(shaderID);

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    void InitQuad()
    {
        float vertices[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    static const std::array<uint8_t, 7>& GetGlyph(char c)
    {
        static const std::array<uint8_t, 7> SPACE = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        static const std::array<uint8_t, 7> COLON = { 0x00, 0x04, 0x04, 0x00, 0x04, 0x04, 0x00 };
        static const std::array<uint8_t, 7> EXCL = { 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04 };
        static const std::array<uint8_t, 7> DIGITS[10] = {
            { 0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E }, // 0
            { 0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E }, // 1
            { 0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F }, // 2
            { 0x1F, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0E }, // 3
            { 0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02 }, // 4
            { 0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E }, // 5
            { 0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E }, // 6
            { 0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08 }, // 7
            { 0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E }, // 8
            { 0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C }  // 9
        };
        static const std::array<uint8_t, 7> LETTERS[26] = {
            { 0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11 }, // A
            { 0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E }, // B
            { 0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E }, // C
            { 0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C }, // D
            { 0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F }, // E
            { 0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10 }, // F
            { 0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F }, // G
            { 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11 }, // H
            { 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F }, // I
            { 0x07, 0x02, 0x02, 0x02, 0x12, 0x12, 0x0C }, // J
            { 0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11 }, // K
            { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F }, // L
            { 0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11 }, // M
            { 0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11 }, // N
            { 0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E }, // O
            { 0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10 }, // P
            { 0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D }, // Q
            { 0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11 }, // R
            { 0x0E, 0x11, 0x10, 0x0E, 0x01, 0x11, 0x0E }, // S
            { 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 }, // T
            { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E }, // U
            { 0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04 }, // V
            { 0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11 }, // W
            { 0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11 }, // X
            { 0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04 }, // Y
            { 0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F }  // Z
        };

        if (c >= '0' && c <= '9') return DIGITS[c - '0'];
        if (c >= 'A' && c <= 'Z') return LETTERS[c - 'A'];
        if (c == ':') return COLON;
        if (c == '!') return EXCL;
        return SPACE;
    }
};