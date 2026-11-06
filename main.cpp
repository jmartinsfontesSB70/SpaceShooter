#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <filesystem>
#include <cmath>
#include <string>
#include <stdexcept>
#include <fstream>

// =========================================
// ESTRUTURA DO TIRO LASER
// =========================================
struct Tiro
{
    sf::RectangleShape forma;

    Tiro(float x, float y)
    {
        // Formato alongado do laser
        forma.setSize(sf::Vector2f(5.f, 18.f));

        // Cor ciano brilhante
        forma.setFillColor(sf::Color::Cyan);

        // Contorno branco para destacar o laser
        forma.setOutlineThickness(1.f);
        forma.setOutlineColor(sf::Color::White);

        // Posiciona o tiro
        forma.setPosition(x, y);
    }
};

struct Inimigo
{
    sf::ConvexShape forma;

    float velocidadeX;
    float velocidadeY;
    float direcaoX;

    float tempoZigueZague;
    float intervaloZigueZague;

    bool kamikaze = false;
    bool perseguindo = false;
    float tempoPerseguicao = 0.f;
    float velocidadeKamikaze = 250.f;

    Inimigo(float x, float y, bool ehKamikaze = false)
    {
        kamikaze = ehKamikaze;

        forma.setPointCount(3);

        forma.setPoint(0, sf::Vector2f(20.f, 30.f));
        forma.setPoint(1, sf::Vector2f(0.f, 0.f));
        forma.setPoint(2, sf::Vector2f(40.f, 0.f));

        forma.setFillColor(sf::Color::Red);
        forma.setPosition(x, y);

        // Velocidade vertical
        velocidadeY =
            100.f + static_cast<float>(std::rand() % 101);

        // Velocidade horizontal
        velocidadeX =
            40.f + static_cast<float>(std::rand() % 81);

        // Começa indo para a esquerda ou para a direita
        direcaoX = (std::rand() % 2 == 0) ? -1.f : 1.f;

        tempoZigueZague = 0.f;

        intervaloZigueZague =
            0.5f + static_cast<float>(std::rand() % 101) / 100.f;
    }

    void atualizar(float deltaTempo, sf::Vector2f posicaoNave)
    {
        sf::Vector2f posicao = forma.getPosition();

        if (kamikaze)
        {
            // Aguarda um pouco antes de iniciar a perseguição
            tempoPerseguicao += deltaTempo;

            if (tempoPerseguicao >= 1.f)
            {
                perseguindo = true;
            }

            if (perseguindo)
            {
                // Calcula a direção até a nave
                sf::Vector2f direcao =
                    posicaoNave - forma.getPosition();

                float distancia = std::sqrt(
                    direcao.x * direcao.x +
                    direcao.y * direcao.y
                    );

                if (distancia > 0.f)
                {
                    direcao.x /= distancia;
                    direcao.y /= distancia;

                    posicao.x += direcao.x * velocidadeKamikaze * deltaTempo;
                    posicao.y += direcao.y * velocidadeKamikaze * deltaTempo;
                }
            }
            else
            {
                posicao.y += velocidadeY * deltaTempo;
            }
        }
        else
        {
            // Movimento normal em zigue-zague
            tempoZigueZague += deltaTempo;

            if (tempoZigueZague >= intervaloZigueZague)
            {
                direcaoX *= -1.f;
                tempoZigueZague = 0.f;
            }

            posicao.x += velocidadeX * direcaoX * deltaTempo;
            posicao.y += velocidadeY * deltaTempo;
        }

        forma.setPosition(posicao);
    }
};

struct Boss
{
    sf::Texture textura;
    sf::Sprite sprite;

    float velocidadeX = 180.f;
    float direcaoX = 1.f;

    int vida = 100;

    Boss()
    {
        // Carrega a imagem do chefão
        if (!textura.loadFromFile("assets/images/boss.png"))
        {
            throw std::runtime_error(
                "Erro ao carregar a imagem do chefao!"
                );
        }

        // Associa a textura ao sprite
        sprite.setTexture(textura);

        // Redimensiona o chefão
        sprite.setScale(0.25f, 0.25f);

        // Define a origem no centro da imagem
        sf::FloatRect limites = sprite.getLocalBounds();

        sprite.setOrigin(
            limites.width / 2.f,
            limites.height / 2.f
            );

        // Posição inicial
        sprite.setPosition(400.f, 90.f);
    }

    void atualizar(float deltaTempo)
    {
        sf::Vector2f posicao = sprite.getPosition();

        // Movimento horizontal
        posicao.x += velocidadeX * direcaoX * deltaTempo;

        // Limites da janela
        float metadeLargura =
            sprite.getGlobalBounds().width / 2.f;

        if (posicao.x >= 800.f - metadeLargura)
        {
            posicao.x = 800.f - metadeLargura;
            direcaoX = -1.f;
        }
        else if (posicao.x <= metadeLargura)
        {
            posicao.x = metadeLargura;
            direcaoX = 1.f;
        }

        sprite.setPosition(posicao);
    }
};

struct Boss2
{
    sf::Texture textura;
    sf::Sprite sprite;

    float velocidadeX = 160.f;
    float velocidadeY = 100.f;

    float direcaoX = 1.f;
    float direcaoY = 1.f;

    int vida = 150;

    Boss2()
    {
        if (!textura.loadFromFile("assets/images/boss2.png"))
        {
            throw std::runtime_error(
                "Erro ao carregar a imagem do segundo chefao!"
                );
        }

        sprite.setTexture(textura);

        sprite.setScale(0.15f, 0.15f);

        sf::FloatRect limites = sprite.getLocalBounds();

        sprite.setOrigin(
            limites.width / 2.f,
            limites.height / 2.f
            );

        sprite.setPosition(400.f, 90.f);
    }

    void atualizar(float deltaTempo)
    {
        sf::Vector2f posicao = sprite.getPosition();

        // Movimento diagonal
        posicao.x += velocidadeX * direcaoX * deltaTempo;
        posicao.y += velocidadeY * direcaoY * deltaTempo;

        float metadeLargura =
            sprite.getGlobalBounds().width / 2.f;

        float metadeAltura =
            sprite.getGlobalBounds().height / 2.f;

        // Limites horizontais
        if (posicao.x >= 800.f - metadeLargura)
        {
            posicao.x = 800.f - metadeLargura;
            direcaoX = -1.f;
        }
        else if (posicao.x <= metadeLargura)
        {
            posicao.x = metadeLargura;
            direcaoX = 1.f;
        }

        // Limites verticais
        if (posicao.y >= 220.f - metadeAltura)
        {
            posicao.y = 220.f - metadeAltura;
            direcaoY = -1.f;
        }
        else if (posicao.y <= 60.f + metadeAltura)
        {
            posicao.y = 60.f + metadeAltura;
            direcaoY = 1.f;
        }

        sprite.setPosition(posicao);
    }
};

struct TiroBoss
{
    sf::RectangleShape forma;
    float velocidade = 250.f;

    TiroBoss(float x, float y, float velocidadeTiro)
    {
        velocidade = velocidadeTiro;

        forma.setSize(sf::Vector2f(10.f, 18.f));
        forma.setOrigin(5.f, 9.f);
        forma.setFillColor(sf::Color::Red);
        forma.setPosition(x, y);
    }

    void atualizar(float deltaTempo)
    {
        sf::Vector2f posicao = forma.getPosition();

        posicao.y += velocidade * deltaTempo;

        forma.setPosition(posicao);
    }
};

struct PowerUp
{
    sf::RectangleShape forma;
    float velocidade = 150.f;

    // 1 = tiro duplo
    // 2 = tiro rápido
    int tipo;

    PowerUp(float x, float y, int tipoPowerUp = 1)
    {
        tipo = tipoPowerUp;

        forma.setSize(sf::Vector2f(24.f, 24.f));
        forma.setOrigin(12.f, 12.f);
        forma.setPosition(x, y);

        if (tipo == 1)
        {
            forma.setFillColor(sf::Color::Yellow);
        }
        else if (tipo == 2)
        {
            forma.setFillColor(sf::Color::Magenta);
        }
    }

    void atualizar(float deltaTempo)
    {
        sf::Vector2f posicao = forma.getPosition();

        posicao.y += velocidade * deltaTempo;

        forma.setPosition(posicao);
    }
};

struct VidaExtra
{
    sf::RectangleShape forma;
    float velocidade = 150.f;

    VidaExtra(float x, float y)
    {
        forma.setSize(sf::Vector2f(24.f, 24.f));
        forma.setOrigin(12.f, 12.f);
        forma.setPosition(x, y);
        forma.setFillColor(sf::Color::Red);
    }

    void atualizar(float deltaTempo)
    {
        sf::Vector2f posicao = forma.getPosition();

        posicao.y += velocidade * deltaTempo;

        forma.setPosition(posicao);
    }
};

struct Estrela
{
    sf::CircleShape forma;
    float velocidade;

    Estrela(float x, float y, float raio, float vel)
    {
        forma.setRadius(raio);
        forma.setFillColor(sf::Color(200, 200, 255));
        forma.setPosition(x, y);

        velocidade = vel;
    }
};

// =========================================
// ESTRUTURA DA EXPLOSÃO
// =========================================
struct Particula
{
    sf::CircleShape forma;
    sf::Vector2f velocidade;
    float tempoVida = 0.5f;

    Particula(float x, float y)
    {
        // Tamanho aleatório da partícula
        float raio = 2.f + static_cast<float>(std::rand() % 4);

        forma.setRadius(raio);
        forma.setOrigin(raio, raio);
        forma.setPosition(x, y);

        // Cor inicial: amarelo
        forma.setFillColor(sf::Color::Yellow);

        // Direção aleatória em 360 graus
        float angulo =
            static_cast<float>(std::rand() % 360) * 3.14159265f / 180.f;

        // Velocidade aleatória
        float velocidadeParticula =
            100.f + static_cast<float>(std::rand() % 201);

        velocidade.x = std::cos(angulo) * velocidadeParticula;
        velocidade.y = std::sin(angulo) * velocidadeParticula;
    }

    void atualizar(float deltaTempo)
    {
        // Espalha a partícula pela tela
        forma.move(
            velocidade.x * deltaTempo,
            velocidade.y * deltaTempo
            );

        // A partícula perde velocidade gradualmente
        velocidade.x *= 0.98f;
        velocidade.y *= 0.98f;

        // Diminui o tempo de vida
        tempoVida -= deltaTempo;

        // Calcula a transparência
        float proporcao = std::max(0.f, tempoVida / 0.5f);

        sf::Uint8 alpha = static_cast<sf::Uint8>(
            255.f * proporcao
            );

        // Amarelo vira laranja enquanto desaparece
        sf::Uint8 verde = static_cast<sf::Uint8>(
            165.f * proporcao
            );

        forma.setFillColor(
            sf::Color(255, verde, 0, alpha)
            );
    }

    bool terminou() const
    {
        return tempoVida <= 0.f;
    }
};

struct Explosao
{
    std::vector<Particula> particulas;

    Explosao(float x, float y)
    {
        // Cria 20 partículas no ponto da explosão
        for (int i = 0; i < 20; i++)
        {
            particulas.emplace_back(x, y);
        }
    }

    void atualizar(float deltaTempo)
    {
        for (auto& particula : particulas)
        {
            particula.atualizar(deltaTempo);
        }

        // Remove as partículas que terminaram
        particulas.erase(
            std::remove_if(
                particulas.begin(),
                particulas.end(),
                [](const Particula& particula)
                {
                    return particula.terminou();
                }
                ),
            particulas.end()
            );
    }

    bool terminou() const
    {
        return particulas.empty();
    }
};

int main()
{
    sf::RenderWindow janela(
        sf::VideoMode(800, 600),
        "Space Shooter"
        );

    janela.setFramerateLimit(60);

    // Carrega a textura da nave
    sf::Texture texturaNave;

    std::cout << "Diretorio atual: "
              << std::filesystem::current_path()
              << '\n';

    std::cout << "Imagem existe? "
              << std::filesystem::exists("assets/images/nave.png")
              << '\n';

    if (!texturaNave.loadFromFile("assets/images/nave.png"))
    {
        std::cerr << "Erro ao carregar a imagem da nave!\n";
        return 1;
    }

    // Cria o sprite da nave
    sf::Sprite nave;
    nave.setTexture(texturaNave);

    // Redimensiona a nave
    nave.setScale(0.15f, 0.15f);

    // Define a origem no centro da imagem
    sf::FloatRect limitesNave = nave.getLocalBounds();

    nave.setOrigin(
        limitesNave.width / 2.f,
        limitesNave.height / 2.f
        );

    // Posiciona a nave no centro da tela
    nave.setPosition(400.f, 300.f);

    std::vector<Tiro> tiros;

    std::vector<Inimigo> inimigos;

    std::vector<Explosao> explosoes;

    std::vector<TiroBoss> tirosBoss;

    std::vector<TiroBoss> tirosBoss2;

    std::vector<PowerUp> powerUps;

    std::vector<VidaExtra> vidasExtras;

    sf::Clock relogioQuadro;

    bool jogoAtivo = false;
    bool telaInicial = true;
    bool jogoPausado = false;

    Boss boss;
    bool bossAtivo = false;
    int faseBoss = 1;

    Boss2 boss2;
    bool boss2Ativo = false;
    int faseBoss2 = 1;

    float tempoTiroBoss2 = 0.f;
    float intervaloTiroBoss2 = 2.f;

    bool aguardandoProximoBoss = false;
    float tempoProximoBoss = 0.f;

    //bool boss1Derrotado = false;
    //bool boss2Derrotado = false;

    bool proximoBoss2 = false;

    sf::Clock relogioTiroBoss;
    float intervaloTiroBoss = 1.5f;

    // Controle do escudo da nave
    bool escudoAtivo = false;
    //sf::Clock relogioEscudo;

    // Power-up de tiro duplo
    bool tiroDuploAtivo = false;
    //sf::Clock relogioTiroDuplo;

    int inimigosDerrotados = 0;
    //bool bossJaApareceu = false;

    // Power-up de tiro rápido
    bool tiroRapidoAtivo = false;
    //sf::Clock relogioTiroRapido;

    // Controle do intervalo entre disparos
    //sf::Clock relogioDisparo;
    float intervaloDisparo = 0.25f;

    //sf::Clock relogioInimigos;

    // =========================================
    // TEMPORIZADORES DO JOGO
    // =========================================
    float tempoDisparo = 0.f;
    float tempoInimigos = 0.f;
    float tempoTiroBoss = 0.f;

    float tempoEscudo = 0.f;
    float tempoTiroDuplo = 0.f;
    float tempoTiroRapido = 0.f;

    float velocidadeInimigo = 2.f;

    float velocidadeTiro = 7.f;

    bool espacoPressionado = false;

    // Controle de dificuldade
    int nivelDificuldade = 1;
    float intervaloInimigos = 1.0f;
    float multiplicadorVelocidade = 1.0f;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int pontuacao = 0;

    int vidas = 3;

    int proximaPontuacaoBoss = 250;

    int highScore = 0;

    int ondaAtual = 1;
    int inimigosCriadosNaOnda = 0;
    int totalInimigosNaOnda = 5;
    bool aguardandoProximaOnda = false;
    float tempoProximaOnda = 0.f;

    std::cout << "Fonte existe? "
              << std::filesystem::exists("assets/fonts/arial.ttf")
              << '\n';


    sf::Font fonte;

    if (!fonte.loadFromFile("assets/fonts/arial.ttf"))
    {
        std::cerr << "Erro ao carregar a fonte!\n";
        return 1;
    }

    sf::Text textoPontuacao;

    textoPontuacao.setFont(fonte);
    textoPontuacao.setCharacterSize(24);
    textoPontuacao.setFillColor(sf::Color::White);
    textoPontuacao.setPosition(10.f, 10.f);

    sf::Text textoGameOver;

    textoGameOver.setFont(fonte);
    textoGameOver.setString("GAME OVER");
    textoGameOver.setCharacterSize(50);
    textoGameOver.setFillColor(sf::Color::Red);
    textoGameOver.setPosition(250.f, 250.f);

    sf::Text textoReiniciar;

    textoReiniciar.setFont(fonte);
    textoReiniciar.setString("Pressione R para reiniciar");
    textoReiniciar.setCharacterSize(22);
    textoReiniciar.setFillColor(sf::Color::White);
    textoReiniciar.setPosition(230.f, 340.f);

    sf::Text textoVidas;

    textoVidas.setFont(fonte);
    textoVidas.setCharacterSize(24);
    textoVidas.setFillColor(sf::Color::White);
    textoVidas.setPosition(650.f, 10.f);

    sf::Text textoNivel;

    textoNivel.setFont(fonte);
    textoNivel.setCharacterSize(24);
    textoNivel.setFillColor(sf::Color::Yellow);
    textoNivel.setPosition(10.f, 40.f);

    sf::Text textoHighScore;

    textoHighScore.setFont(fonte);
    textoHighScore.setCharacterSize(20);
    textoHighScore.setFillColor(sf::Color::Yellow);
    textoHighScore.setPosition(10.f, 70.f);

    sf::Text textoTiroDuplo;

    textoTiroDuplo.setFont(fonte);
    textoTiroDuplo.setCharacterSize(20);
    textoTiroDuplo.setFillColor(sf::Color::Cyan);
    textoTiroDuplo.setPosition(10.f, 95.f);

    sf::Text textoTiroRapido;

    textoTiroRapido.setFont(fonte);
    textoTiroRapido.setCharacterSize(20);
    textoTiroRapido.setFillColor(sf::Color::Magenta);
    textoTiroRapido.setPosition(10.f, 115.f);

    sf::Text textoOnda;

    textoOnda.setFont(fonte);
    textoOnda.setCharacterSize(22);
    textoOnda.setFillColor(sf::Color::White);
    textoOnda.setPosition(sf::Vector2f(650.f, 70.f));

    // Texto de transição entre ondas
    sf::Text textoTransicao;
    textoTransicao.setFont(fonte);
    textoTransicao.setCharacterSize(40);
    textoTransicao.setFillColor(sf::Color::Yellow);
    textoTransicao.setString("");

    // =========================================
    // TEXTOS DA TELA INICIAL
    // =========================================

    sf::Text tituloJogo;
    tituloJogo.setFont(fonte);
    tituloJogo.setString("SPACE SHOOTER");
    tituloJogo.setCharacterSize(48);
    tituloJogo.setFillColor(sf::Color::Cyan);
    tituloJogo.setPosition(190.f, 180.f);

    sf::Text instrucaoInicial;
    instrucaoInicial.setFont(fonte);
    instrucaoInicial.setString("PRESSIONE ENTER PARA JOGAR");
    instrucaoInicial.setCharacterSize(22);
    instrucaoInicial.setFillColor(sf::Color::White);
    instrucaoInicial.setPosition(190.f, 300.f);

    sf::Text textoRecordeMenu;
    textoRecordeMenu.setFont(fonte);
    //textoRecordeMenu.setString("PRESSIONE ENTER PARA JOGAR");
    textoRecordeMenu.setCharacterSize(22);
    textoRecordeMenu.setFillColor(sf::Color::White);
    textoRecordeMenu.setPosition(280.f, 250.f);

    std::vector<Estrela> estrelas;

    for (int i = 0; i < 100; i++)
    {
        float x = static_cast<float>(std::rand() % 800);
        float y = static_cast<float>(std::rand() % 600);

        float raio = 1.f + static_cast<float>(std::rand() % 3);
        float velocidade = 0.5f + static_cast<float>(std::rand() % 3);

        estrelas.emplace_back(x, y, raio, velocidade);
    }

    const float FATOR_COLISAO_NAVE = 0.70f;
    const float FATOR_COLISAO_INIMIGO = 0.70f;

    // Carrega o recorde salvo em arquivo
    std::ifstream arquivoHighScore("highscore.txt");

    if (arquivoHighScore.is_open())
    {
        arquivoHighScore >> highScore;
        arquivoHighScore.close();
    }

    while (janela.isOpen())
    {
        float deltaTempo =
            relogioQuadro.restart().asSeconds();

        // Acumula os tempos somente durante a partida ativa
        if (jogoAtivo && !jogoPausado)
        {
            tempoDisparo += deltaTempo;
            tempoInimigos += deltaTempo;

            if (bossAtivo)
            {
                tempoTiroBoss += deltaTempo;
            }

            if (boss2Ativo)
            {
                tempoTiroBoss2 += deltaTempo;
            }

            if (escudoAtivo)
            {
                tempoEscudo += deltaTempo;
            }

            if (tiroDuploAtivo)
            {
                tempoTiroDuplo += deltaTempo;
            }

            if (tiroRapidoAtivo)
            {
                tempoTiroRapido += deltaTempo;
            }
        }

        // =========================================
        // 1. PROCESSAR EVENTOS
        // =========================================
        sf::Event evento;

        while (janela.pollEvent(evento))
        {
            if (evento.type == sf::Event::Closed)
            {
                janela.close();
            }
            else if (evento.type == sf::Event::KeyPressed &&
                     evento.key.code == sf::Keyboard::R &&
                     !jogoAtivo && !telaInicial)
            {

                inimigosDerrotados = 0;

                bossAtivo = false;
                boss2Ativo = false;

                //bossJaApareceu = false;

                proximoBoss2 = false;

                boss.vida = 100;
                boss2.vida = 150;

                boss.sprite.setPosition(400.f, 90.f);
                boss2.sprite.setPosition(400.f, 90.f);

                tempoTiroBoss = 0.f;
                tempoTiroBoss2 = 0.f;

                aguardandoProximoBoss = false;
                tempoProximoBoss = 0.f;

                proximaPontuacaoBoss = 250;

                faseBoss = 1;
                faseBoss2 = 1;

                tiros.clear();
                tirosBoss.clear();
                tirosBoss2.clear();

                inimigos.clear();
                explosoes.clear();

                pontuacao = 0;

                vidas = 3;

                nave.setPosition(400.f, 300.f);

                tempoInimigos = 0.f;
                tempoDisparo = 0.f;
                tempoTiroBoss = 0.f;
                tempoEscudo = 0.f;
                tempoTiroDuplo = 0.f;
                tempoTiroRapido = 0.f;

                ondaAtual = 1;
                inimigosCriadosNaOnda = 0;
                totalInimigosNaOnda = 5;
                aguardandoProximaOnda = false;
                tempoProximaOnda = 0.f;

                multiplicadorVelocidade = 1.0f;

                escudoAtivo = false;
                tiroDuploAtivo = false;
                tiroRapidoAtivo = false;
                jogoPausado = false;

                jogoAtivo = true;
            }
            else if (evento.type == sf::Event::KeyPressed &&
                    evento.key.code == sf::Keyboard::Enter &&
                    telaInicial)
            {

                inimigosDerrotados = 0;
                bossAtivo = false;
                //bossJaApareceu = false;

                tempoTiroBoss = 0.f;
                tempoTiroBoss2 = 0.f;

                boss.vida = 100;
                boss.sprite.setPosition(400.f, 90.f);
                boss2.sprite.setPosition(400.f, 90.f);

                tempoTiroBoss = 0.f;
                tempoTiroBoss2 = 0.f;

                aguardandoProximoBoss = false;
                tempoProximoBoss = 0.f;

                proximaPontuacaoBoss = 250;

                telaInicial = false;
                jogoAtivo = true;

                pontuacao = 0;
                vidas = 3;

                tiros.clear();
                tirosBoss.clear();
                tirosBoss2.clear();

                inimigos.clear();
                explosoes.clear();

                tempoInimigos = 0.f;
                tempoDisparo = 0.f;
                tempoTiroBoss = 0.f;
                tempoEscudo = 0.f;
                tempoTiroDuplo = 0.f;
                tempoTiroRapido = 0.f;

                ondaAtual = 1;
                inimigosCriadosNaOnda = 0;
                totalInimigosNaOnda = 5;
                aguardandoProximaOnda = false;
                tempoProximaOnda = 0.f;

                multiplicadorVelocidade = 1.0f;

                escudoAtivo = false;
                tiroDuploAtivo = false;
                tiroRapidoAtivo = false;
                jogoPausado = false;

            }
            else if (evento.type == sf::Event::KeyPressed &&
                     evento.key.code == sf::Keyboard::E &&
                     jogoAtivo &&
                     !escudoAtivo)
            {
                escudoAtivo = true;
                tempoEscudo = 0.f;
            }
            else if (evento.type == sf::Event::KeyPressed &&
                     evento.key.code == sf::Keyboard::P &&
                     jogoAtivo)
            {
                jogoPausado = !jogoPausado;
            }
        }

        // =========================================
        // 2. MOVIMENTAÇÃO DA NAVE
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            float velocidade = 4.0f;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                nave.move(-velocidade, 0.f);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                nave.move(velocidade, 0.f);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                nave.move(0.f, -velocidade);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                nave.move(0.f, velocidade);

            // =========================================
            // 3. LIMITES DA NAVE
            // =========================================

            // Mantém a nave dentro dos limites da janela
            sf::Vector2f posicao = nave.getPosition();

            float metadeLargura = nave.getGlobalBounds().width / 2.f;
            float metadeAltura = nave.getGlobalBounds().height / 2.f;

            // Limites esquerdo e direito
            if (posicao.x < metadeLargura)
                posicao.x = metadeLargura;

            if (posicao.x > 800.f - metadeLargura)
                posicao.x = 800.f - metadeLargura;

            // Limites superior e inferior
            if (posicao.y < metadeAltura)
                posicao.y = metadeAltura;

            if (posicao.y > 600.f - metadeAltura)
                posicao.y = 600.f - metadeAltura;

            nave.setPosition(posicao);

            // =========================================
            // 4. DISPARAR
            // =========================================
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                float intervaloAtual = tiroRapidoAtivo ? 0.10f : intervaloDisparo;

               if (tempoDisparo >= intervaloAtual)
                {
                    sf::FloatRect limitesNave = nave.getGlobalBounds();

                    float centroX = limitesNave.left + limitesNave.width / 2.f;
                    float pontaY = limitesNave.top;

                    if (tiroDuploAtivo)
                    {
                        tiros.emplace_back(
                            centroX - 12.5f,
                            pontaY
                            );

                        tiros.emplace_back(
                            centroX + 7.5f,
                            pontaY
                            );
                    }
                    else
                    {
                        tiros.emplace_back(
                            centroX - 2.5f,
                            pontaY
                            );
                    }

                   tempoDisparo = 0.f;
                }
            }
        }

        // =========================================
        // 5. CRIAR INIMIGOS
        // =========================================
        if (jogoAtivo && !jogoPausado &&
            !aguardandoProximaOnda &&
            inimigosCriadosNaOnda < totalInimigosNaOnda &&
            tempoInimigos >= intervaloInimigos)
        {
            float x = static_cast<float>(std::rand() % 680);

            bool kamikaze1 = (std::rand() % 100) < 20;
            bool kamikaze2 = (std::rand() % 100) < 20;
            bool kamikaze3 = (std::rand() % 100) < 20;

            inimigos.emplace_back(x, -30.f, kamikaze1);
            inimigos.emplace_back(x + 45.f, -30.f, kamikaze2);
            inimigos.emplace_back(x + 90.f, -30.f, kamikaze3);

            inimigosCriadosNaOnda += 3;

            tempoInimigos = 0.f;
        }
        // =========================================
        // ATUALIZAÇÃO DO CHEFÃO
        // =========================================
        if (jogoAtivo && bossAtivo && !jogoPausado)
        {
            boss.atualizar(deltaTempo);

            // Define a fase de combate do chefe
            if (boss.vida > 70)
            {
                faseBoss = 1;
            }
            else if (boss.vida > 35)
            {
                faseBoss = 2;
            }
            else
            {
                faseBoss = 3;
            }

            // Define a frequência dos disparos conforme a fase
            float intervaloAtualBoss = intervaloTiroBoss;

            if (faseBoss == 2)
            {
                intervaloAtualBoss *= 0.75f;
            }
            else if (faseBoss == 3)
            {
                intervaloAtualBoss *= 0.50f;
            }

            if (tempoTiroBoss >= intervaloAtualBoss)
            {
                sf::Vector2f posicao = boss.sprite.getPosition();

                float velocidadeTiroBoss = 250.f * multiplicadorVelocidade;

                tirosBoss.emplace_back(
                    posicao.x,
                    posicao.y + 35.f,
                    velocidadeTiroBoss
                    );

                tempoTiroBoss = 0.f;
            }
        }

        // =========================================
        // ATUALIZAÇÃO DO SEGUNDO CHEFÃO
        // =========================================
        if (jogoAtivo && boss2Ativo && !jogoPausado)
        {
            boss2.atualizar(deltaTempo);

            if (boss2.vida > 100)
                faseBoss2 = 1;
            else if (boss2.vida > 50)
                faseBoss2 = 2;
            else
                faseBoss2 = 3;

            float intervaloAtualBoss2 = intervaloTiroBoss2;

            if (faseBoss2 == 2)
                intervaloAtualBoss2 *= 0.75f;
            else if (faseBoss2 == 3)
                intervaloAtualBoss2 *= 0.50f;

            if (tempoTiroBoss2 >= intervaloAtualBoss2)
            {
                sf::Vector2f posicao = boss2.sprite.getPosition();

                float velocidadeTiroBoss2 =
                    250.f * multiplicadorVelocidade;

                tirosBoss2.emplace_back(
                    posicao.x,
                    posicao.y + 35.f,
                    velocidadeTiroBoss2
                    );

                tempoTiroBoss2 = 0.f;
            }
        }

        // =========================================
        // 6. MOVIMENTAR OS TIROS
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            for (auto& tiro : tiros)
            {
                tiro.forma.move(0.f, -velocidadeTiro);
            }

            // Movimentar os tiros do chefão
            for (auto& tiro : tirosBoss)
            {
                tiro.atualizar(deltaTempo);
            }

            // Movimentar os tiros do segundo chefão
            for (auto& tiro : tirosBoss2)
            {
                tiro.atualizar(deltaTempo);
            }
        }

        // =========================================
        // MOVIMENTAR OS POWER-UPS
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            for (auto& powerUp : powerUps)
            {
                powerUp.atualizar(deltaTempo);
            }
        }

        // =========================================
        // REMOVER POWER-UPS FORA DA TELA
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            powerUps.erase(
                std::remove_if(
                    powerUps.begin(),
                    powerUps.end(),
                    [](const PowerUp& powerUp)
                    {
                        return powerUp.forma.getPosition().y > 630.f;
                    }
                    ),
                powerUps.end()
                );
        }

        // =========================================
        // MOVIMENTAR AS VIDAS EXTRAS
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            for (auto& vidaExtra : vidasExtras)
            {
                vidaExtra.atualizar(deltaTempo);
            }
        }

        // =========================================
        // REMOVER VIDAS EXTRAS FORA DA TELA
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            vidasExtras.erase(
                std::remove_if(
                    vidasExtras.begin(),
                    vidasExtras.end(),
                    [](const VidaExtra& vidaExtra)
                    {
                        return vidaExtra.forma.getPosition().y > 630.f;
                    }
                    ),
                vidasExtras.end()
                );
        }

        // =========================================
        // 7. REMOVER TIROS QUE SAÍRAM DA TELA
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            tiros.erase(
                std::remove_if(
                    tiros.begin(),
                    tiros.end(),
                    [](const Tiro& tiro)
                    {
                        return tiro.forma.getPosition().y
                                   + tiro.forma.getSize().y < 0.f;
                    }
                    ),
                tiros.end()
                );
        }

        // =========================================
        // 7.1 REMOVER TIROS DO CHEFÃO QUE SAÍRAM DA TELA
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            tirosBoss.erase(
                std::remove_if(
                    tirosBoss.begin(),
                    tirosBoss.end(),
                    [](const TiroBoss& tiro)
                    {
                        return tiro.forma.getPosition().y
                                    - tiro.forma.getOrigin().y > 600.f;
                    }
                    ),
                tirosBoss.end()
                );
        }

        // =========================================
        // 10. COLISÃO ENTRE TIROS E INIMIGOS
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            for (auto tiro = tiros.begin(); tiro != tiros.end(); )
            {
                bool tiroAcertou = false;

                for (auto inimigo = inimigos.begin();
                     inimigo != inimigos.end(); )
                {
                    if (tiro->forma.getGlobalBounds().intersects(
                            inimigo->forma.getGlobalBounds()))
                    {
                        pontuacao += 10;

                        inimigosDerrotados++;

                        // Chance de 30% de deixar cair um power-up
                        if (std::rand() % 100 < 30)
                        {
                            sf::Vector2f posicaoInimigo = inimigo->forma.getPosition();

                            // Sorteia o tipo: 1 = tiro duplo, 2 = tiro rápido
                            int tipoPowerUp = (std::rand() % 2) + 1;

                            powerUps.emplace_back(
                                posicaoInimigo.x + 20.f,
                                posicaoInimigo.y + 15.f,
                                tipoPowerUp
                                );
                        }

                        // Chance de 10% de deixar cair uma vida extra
                        if (std::rand() % 100 < 10)
                        {
                            sf::Vector2f posicaoInimigo = inimigo->forma.getPosition();

                            vidasExtras.emplace_back(
                                posicaoInimigo.x + 20.f,
                                posicaoInimigo.y + 15.f
                                );
                        }

                        sf::FloatRect limites =
                            inimigo->forma.getGlobalBounds();

                        explosoes.emplace_back(
                            limites.left + limites.width / 2.f,
                            limites.top + limites.height / 2.f
                            );

                        inimigo = inimigos.erase(inimigo);

                        tiroAcertou = true;

                        break;
                    }
                    else
                    {
                        ++inimigo;
                    }
                }

                if (tiroAcertou)
                {
                    tiro = tiros.erase(tiro);
                }
                else
                {
                    ++tiro;
                }
            }
        }

        // =========================================
        // CONTROLE DAS ONDAS
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            // Verifica se a onda terminou
            if (inimigosCriadosNaOnda >= totalInimigosNaOnda &&
                inimigos.empty() &&
                !aguardandoProximaOnda)
            {
                aguardandoProximaOnda = true;
                tempoProximaOnda = 3.f;
                textoTransicao.setString(
                    "ONDA " + std::to_string(ondaAtual + 1) +
                    "\nPREPARE-SE!"
                    );
                sf::FloatRect limitesTexto =
                    textoTransicao.getLocalBounds();

                textoTransicao.setOrigin(
                    sf::Vector2f(
                        limitesTexto.left + limitesTexto.width / 2.f,
                        limitesTexto.top + limitesTexto.height / 2.f
                        )
                    );

                textoTransicao.setPosition(
                    sf::Vector2f(400.f, 300.f)
                    );
            }

            // Aguarda antes de iniciar a próxima onda
            if (aguardandoProximaOnda)
            {
                tempoProximaOnda -= deltaTempo;

                if (tempoProximaOnda <= 0.f)
                {
                    ondaAtual++;

                    textoTransicao.setString("");

                    // Aumenta a velocidade a cada nova onda
                    multiplicadorVelocidade += 0.10f;

                    inimigosCriadosNaOnda = 0;

                    totalInimigosNaOnda += 5;

                    aguardandoProximaOnda = false;

                    tempoInimigos = 0.f;
                }
            }
        }

        // =========================================
        // ATUALIZAR NÍVEL DE DIFICULDADE
        // =========================================
        if (pontuacao >= 1000)
        {
            nivelDificuldade = 3;
            intervaloInimigos = 0.5f;
            multiplicadorVelocidade = 1.5f;
            intervaloTiroBoss = 0.75f;
        }
        else if (pontuacao >= 500)
        {
            nivelDificuldade = 2;
            intervaloInimigos = 0.75f;
            multiplicadorVelocidade = 1.25f;
            intervaloTiroBoss = 1.0f;
        }
        else
        {
            nivelDificuldade = 1;
            intervaloInimigos = 1.0f;
            multiplicadorVelocidade = 1.0f;
            intervaloTiroBoss = 1.5f;
        }

        // =========================================
        // CONTROLAR DURAÇÃO DO TIRO DUPLO
        // =========================================

        // Controle do tempo do tiro duplo
        if (tiroDuploAtivo && !jogoPausado &&
                tempoTiroDuplo >= 8.f)
        {
            tiroDuploAtivo = false;
        }

        // Controle do tempo do tiro rápido
        if (tiroRapidoAtivo && !jogoPausado &&
            tempoTiroRapido >= 8.f)
        {
            tiroRapidoAtivo = false;
        }

        // =========================================
        // COLISÃO ENTRE TIROS E CHEFÃO
        // =========================================
        if (jogoAtivo && bossAtivo && !jogoPausado)
        {
            for (auto tiro = tiros.begin(); tiro != tiros.end(); )
            {
                if (tiro->forma.getGlobalBounds().intersects(
                        boss.sprite.getGlobalBounds()))
                {
                    // O chefão perde um ponto de vida
                    boss.vida--;

                    // Obtém a posição do impacto
                    sf::Vector2f posicao = tiro->forma.getPosition();

                    // Cria uma explosão no local do impacto
                    explosoes.emplace_back(posicao.x, posicao.y);

                    // Remove o tiro que acertou
                    tiro = tiros.erase(tiro);

                    // Verifica se o chefão foi derrotado
                    if (boss.vida <= 0)
                    {
                        // Centro do chefão
                        sf::Vector2f posicao = boss.sprite.getPosition();

                        // Cria a explosão
                        explosoes.emplace_back(
                            posicao.x,
                            posicao.y
                            );

                        // Recompensa por derrotar o chefão
                        pontuacao += 100;

                        // Desativa o chefão
                        bossAtivo = false;

                        aguardandoProximoBoss = true;
                        tempoProximoBoss = 4.f;

                        // Registra a derrota do primeiro chefe
                        //boss1Derrotado = true;

                        // Encerra a verificação dos tiros
                        break;
                    }
                }
                else
                {
                    ++tiro;
                }
            }
        }

        // =========================================
        // COLISÃO ENTRE TIROS E SEGUNDO CHEFÃO
        // =========================================
        if (jogoAtivo && boss2Ativo && !jogoPausado)
        {
            for (auto tiro = tiros.begin(); tiro != tiros.end(); )
            {
                if (tiro->forma.getGlobalBounds().intersects(
                        boss2.sprite.getGlobalBounds()))
                {
                    // O segundo chefão perde um ponto de vida
                    boss2.vida--;

                    // Obtém a posição do impacto
                    sf::Vector2f posicao = tiro->forma.getPosition();

                    // Cria uma explosão no local do impacto
                    explosoes.emplace_back(posicao.x, posicao.y);

                    // Remove o tiro que acertou
                    tiro = tiros.erase(tiro);

                    // Verifica se o segundo chefão foi derrotado
                    if (boss2.vida <= 0)
                    {
                        // Centro do segundo chefão
                        sf::Vector2f posicaoBoss2 = boss2.sprite.getPosition();

                        // Cria a explosão
                        explosoes.emplace_back(
                            posicaoBoss2.x,
                            posicaoBoss2.y
                            );

                        // Recompensa por derrotar o segundo chefão
                        pontuacao += 200;

                        // Desativa o segundo chefão
                        boss2Ativo = false;

                        aguardandoProximoBoss = true;
                        tempoProximoBoss = 4.f;

                        // Registra a derrota
                        //boss2Derrotado = true;

                        // Encerra a verificação dos tiros
                        break;
                    }
                }
                else
                {
                    ++tiro;
                }
            }
        }

        // =========================================
        // CONTROLAR DURAÇÃO DO ESCUDO
        // =========================================
        if (escudoAtivo && !jogoPausado &&
            tempoEscudo >= 2.f)
        {
            escudoAtivo = false;
        }

        // =========================================
        // EFEITO VISUAL DO ESCUDO
        // =========================================
        if (escudoAtivo)
        {
            if (!jogoPausado)
            {
                if (static_cast<int>(tempoEscudo * 10) % 2 == 0)
                {
                    nave.setColor(sf::Color(100, 200, 255));
                }
                else
                {
                    nave.setColor(sf::Color::White);
                }
            }
        }
        else
        {
            nave.setColor(sf::Color::White);
        }

        // =========================================
        // 8. MOVIMENTAR OS INIMIGOS
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            for (auto& inimigo : inimigos)
            {
                inimigo.atualizar(
                    deltaTempo * multiplicadorVelocidade,
                    nave.getPosition()
                    );
            }
        }

        // =========================================
        // 9. REMOVER INIMIGOS QUE SAÍRAM DA TELA
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            inimigos.erase(
                std::remove_if(
                    inimigos.begin(),
                    inimigos.end(),
                    [](const Inimigo& inimigo)
                    {
                        return inimigo.forma.getPosition().y > 600.f;
                    }
                    ),
                inimigos.end()
                );
        }

        // =========================================
        // COLISÃO ENTRE NAVE E INIMIGOS
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            for (auto inimigo = inimigos.begin();
                 inimigo != inimigos.end(); )
            {
                sf::FloatRect limitesNave =
                    nave.getGlobalBounds();

                sf::FloatRect limitesInimigo =
                    inimigo->forma.getGlobalBounds();

                // Reduz a área de colisão para acompanhar melhor
                // o formato visual da nave e do inimigo
                limitesNave.left += limitesNave.width * 0.15f;
                limitesNave.width *= 0.70f;

                limitesNave.top += limitesNave.height * 0.15f;
                limitesNave.height *= 0.70f;

                limitesInimigo.left += limitesInimigo.width * 0.15f;
                limitesInimigo.width *= 0.70f;

                limitesInimigo.top += limitesInimigo.height * 0.15f;
                limitesInimigo.height *= 0.70f;

                if (limitesNave.intersects(limitesInimigo) && !escudoAtivo)
                {
                    // Cria uma explosão na posição da nave
                    sf::Vector2f posicaoNave = nave.getPosition();

                    explosoes.emplace_back(
                        posicaoNave.x,
                        posicaoNave.y
                        );

                    // Desconta uma vida
                    vidas--;

                    // Remove o inimigo que atingiu a nave
                    inimigo = inimigos.erase(inimigo);

                    // Game Over somente quando não houver vidas
                    if (vidas <= 0)
                    {
                        // Atualiza e salva o recorde
                        if (pontuacao > highScore)
                        {
                            highScore = pontuacao;

                            std::ofstream arquivoHighScore("highscore.txt");

                            if (arquivoHighScore.is_open())
                            {
                                arquivoHighScore << highScore;
                                arquivoHighScore.close();
                            }
                        }

                        jogoAtivo = false;
                    }

                    // Uma colisão por vez nesta verificação
                    break;
                }
                else
                {
                    ++inimigo;
                }
            }
        }

        // =========================================
        // COLISÃO ENTRE TIROS DO CHEFÃO E NAVE
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            for (auto tiro = tirosBoss.begin();
                 tiro != tirosBoss.end(); )
            {
                sf::FloatRect limitesNave =
                    nave.getGlobalBounds();

                sf::FloatRect limitesTiro =
                    tiro->forma.getGlobalBounds();

                // Reduz a área de colisão da nave
                limitesNave.left += limitesNave.width * 0.15f;
                limitesNave.width *= 0.70f;

                limitesNave.top += limitesNave.height * 0.15f;
                limitesNave.height *= 0.70f;

                if (limitesNave.intersects(limitesTiro) && !escudoAtivo)
                {
                    // Cria uma explosão na posição da nave
                    sf::Vector2f posicaoNave = nave.getPosition();

                    explosoes.emplace_back(
                        posicaoNave.x,
                        posicaoNave.y
                        );

                    // Desconta uma vida
                    vidas--;

                    // Remove o projétil que atingiu a nave
                    tiro = tirosBoss.erase(tiro);

                    // Game Over quando não houver vidas
                    if (vidas <= 0)
                    {
                        // Atualiza e salva o recorde
                        if (pontuacao > highScore)
                        {
                            highScore = pontuacao;

                            std::ofstream arquivoHighScore("highscore.txt");

                            if (arquivoHighScore.is_open())
                            {
                                arquivoHighScore << highScore;
                                arquivoHighScore.close();
                            }
                        }

                        jogoAtivo = false;
                    }

                    // Uma colisão por vez nesta verificação
                    break;
                }
                else
                {
                    ++tiro;
                }
            }
        }

        // =========================================
        // COLISÃO ENTRE TIROS DO SEGUNDO CHEFÃO E NAVE
        // =========================================
        if (jogoAtivo && boss2Ativo && !jogoPausado)
        {
            for (auto tiro = tirosBoss2.begin();
                 tiro != tirosBoss2.end(); )
            {
                sf::FloatRect limitesNave =
                    nave.getGlobalBounds();

                sf::FloatRect limitesTiro =
                    tiro->forma.getGlobalBounds();

                // Reduz a área de colisão da nave
                limitesNave.left += limitesNave.width * 0.15f;
                limitesNave.width *= 0.70f;

                limitesNave.top += limitesNave.height * 0.15f;
                limitesNave.height *= 0.70f;

                if (limitesNave.intersects(limitesTiro) && !escudoAtivo)
                {
                    // Cria uma explosão na posição da nave
                    sf::Vector2f posicaoNave = nave.getPosition();

                    explosoes.emplace_back(
                        posicaoNave.x,
                        posicaoNave.y
                        );

                    // Desconta uma vida
                    vidas--;

                    // Remove o projétil que atingiu a nave
                    tiro = tirosBoss2.erase(tiro);

                    // Game Over quando não houver vidas
                    if (vidas <= 0)
                    {
                        // Atualiza e salva o recorde
                        if (pontuacao > highScore)
                        {
                            highScore = pontuacao;

                            std::ofstream arquivoHighScore("highscore.txt");

                            if (arquivoHighScore.is_open())
                            {
                                arquivoHighScore << highScore;
                                arquivoHighScore.close();
                            }
                        }

                        jogoAtivo = false;
                    }

                    // Uma colisão por vez nesta verificação
                    break;
                }
                else
                {
                    ++tiro;
                }
            }
        }

        // =========================================
        // COLISÃO ENTRE NAVE E POWER-UPS
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            for (auto powerUp = powerUps.begin();
                 powerUp != powerUps.end(); )
            {
                if (nave.getGlobalBounds().intersects(
                        powerUp->forma.getGlobalBounds()))
                {
                    if (powerUp->tipo == 1)
                    {
                        tiroDuploAtivo = true;
                        tempoTiroDuplo = 0.f;
                    }
                    else if (powerUp->tipo == 2)
                    {
                        tiroRapidoAtivo = true;
                        tempoTiroRapido = 0.f;
                    }

                    powerUp = powerUps.erase(powerUp);
                }
                else
                {
                    ++powerUp;
                }
            }
        }

        // =========================================
        // COLISÃO ENTRE NAVE E VIDAS EXTRAS
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            for (auto vidaExtra = vidasExtras.begin();
                 vidaExtra != vidasExtras.end(); )
            {
                if (nave.getGlobalBounds().intersects(
                        vidaExtra->forma.getGlobalBounds()))
                {
                    // Recupera uma vida
                    vidas++;

                    // Remove a vida extra coletada
                    vidaExtra = vidasExtras.erase(vidaExtra);
                }
                else
                {
                    ++vidaExtra;
                }
            }
        }

        if (jogoAtivo && !jogoPausado && aguardandoProximoBoss)
        {
            tempoProximoBoss -= deltaTempo;

            if (tempoProximoBoss <= 0.f)
            {
                aguardandoProximoBoss = false;
            }
        }

        // =========================================
        // APARECIMENTO DO CHEFÃO POR PONTUAÇÃO
        // =========================================
        if (jogoAtivo && !jogoPausado &&
            !bossAtivo && !boss2Ativo &&
            !aguardandoProximoBoss &&
            pontuacao >= proximaPontuacaoBoss)
        {
            if (!proximoBoss2)
            {
                bossAtivo = true;
                //boss1Derrotado = false;
                boss.vida = 100;
                boss.sprite.setPosition(400.f, 90.f);
                faseBoss = 1;
                tempoTiroBoss = 0.f;
            }
            else
            {
                boss2Ativo = true;
                //boss2Derrotado = false;
                boss2.vida = 150;
                boss2.sprite.setPosition(400.f, 90.f);
                faseBoss2 = 1;
                tempoTiroBoss2 = 0.f;
            }

            proximoBoss2 = !proximoBoss2;
            proximaPontuacaoBoss += 250;
        }

        // =========================================
        // ATUALIZAÇÃO DAS EXPLOSÕES
        // =========================================
        if (jogoAtivo && !jogoPausado)
        {
            for (auto& explosao : explosoes)
            {
                explosao.atualizar(deltaTempo);
            }

            explosoes.erase(
                std::remove_if(
                    explosoes.begin(),
                    explosoes.end(),
                    [](const Explosao& explosao)
                    {
                        return explosao.terminou();
                    }
                    ),
                explosoes.end()
                );
        }

        // =========================================
        // 12. AS ESTRELAS DESCEM CONTINUAMENTE
        // =========================================
        for (auto& estrela : estrelas)
        {
            estrela.forma.move(0.f, estrela.velocidade);

            if (estrela.forma.getPosition().y > 600.f)
            {
                float x = static_cast<float>(std::rand() % 800);

                estrela.forma.setPosition(x, 0.f);
            }
        }

        // =========================================
        // 12. DESENHAR A TELA
        // =========================================
        janela.clear(sf::Color(5, 5, 25));

        // =========================================
        // DESENHAR A TELA INICIAL
        // =========================================

        for (const auto& estrela : estrelas)
        {
            janela.draw(estrela.forma);
        }

        if (telaInicial)
        {
            janela.draw(tituloJogo);
            janela.draw(instrucaoInicial);
            textoRecordeMenu.setString(
                "Recorde: " + std::to_string(highScore));
            janela.draw(textoRecordeMenu);
        }
        else if (jogoAtivo)
        {
            // Os elementos da partida serão desenhados aqui.

            janela.draw(nave);

            for (const auto& tiro : tiros)
            {
                janela.draw(tiro.forma);
            }

            for (const auto& tiro : tirosBoss)
            {
                janela.draw(tiro.forma);
            }

            // Desenhar os tiros do segundo chefão
            for (const auto& tiro : tirosBoss2)
            {
                janela.draw(tiro.forma);
            }

            for (const auto& inimigo : inimigos)
            {
                janela.draw(inimigo.forma);
            }

            // =========================================
            // DESENHAR POWER-UPS
            // =========================================
            for (const auto& powerUp : powerUps)
            {
                janela.draw(powerUp.forma);
            }

            // =========================================
            // DESENHAR VIDAS EXTRAS
            // =========================================
            for (const auto& vidaExtra : vidasExtras)
            {
                janela.draw(vidaExtra.forma);
            }

            // =========================================
            // DESENHAR CHEFÃO E BARRA DE VIDA
            // =========================================
            if (bossAtivo)
            {
                // Desenha o chefão
                janela.draw(boss.sprite);

                // Fundo da barra de vida
                sf::RectangleShape fundoVida(
                    sf::Vector2f(204.f, 18.f)
                    );

                fundoVida.setPosition(298.f, 20.f);
                fundoVida.setFillColor(sf::Color(60, 60, 60));
                fundoVida.setOutlineThickness(2.f);
                fundoVida.setOutlineColor(sf::Color::White);

                // Barra que representa a vida atual
                float larguraVida = 200.f * boss.vida / 100.f;

                sf::RectangleShape barraVida(
                    sf::Vector2f(larguraVida, 14.f)
                    );

                barraVida.setPosition(300.f, 22.f);
                barraVida.setFillColor(sf::Color::Red);

                // Desenha primeiro o fundo e depois a vida
                janela.draw(fundoVida);
                janela.draw(barraVida);
            }

            if (boss2Ativo)
            {
                // Desenha o segundo chefão
                janela.draw(boss2.sprite);

                // Fundo da barra de vida
                sf::RectangleShape fundoVidaBoss2(
                    sf::Vector2f(204.f, 18.f)
                    );

                fundoVidaBoss2.setPosition(298.f, 20.f);
                fundoVidaBoss2.setFillColor(sf::Color(60, 60, 60));
                fundoVidaBoss2.setOutlineThickness(2.f);
                fundoVidaBoss2.setOutlineColor(sf::Color::White);

                // Barra que representa a vida atual
                float larguraVidaBoss2 = 200.f * boss2.vida / 150.f;

                sf::RectangleShape barraVidaBoss2(
                    sf::Vector2f(larguraVidaBoss2, 14.f)
                    );

                barraVidaBoss2.setPosition(300.f, 22.f);
                barraVidaBoss2.setFillColor(sf::Color(255, 140, 0));

                // Desenha o fundo e depois a vida
                janela.draw(fundoVidaBoss2);
                janela.draw(barraVidaBoss2);
            }

            // =========================================
            // DESENHAR AS PARTÍCULAS DAS EXPLOSÕES
            // =========================================
            for (const auto& explosao : explosoes)
            {
                for (const auto& particula : explosao.particulas)
                {
                    janela.draw(particula.forma);
                }
            }

            // Atualizar e desenhar a pontuação
            textoPontuacao.setString(
                "Pontos: " + std::to_string(pontuacao)
                );

            janela.draw(textoPontuacao);

            // Atualizar e desenhar o nível
            textoNivel.setString(
                "Nivel: " + std::to_string(nivelDificuldade)
                );

            janela.draw(textoNivel);

            textoVidas.setString(
                "Vidas: " + std::to_string(vidas)
                );

            janela.draw(textoVidas);

            textoHighScore.setString(
                "Recorde: " + std::to_string(highScore));

            janela.draw(textoHighScore);

            textoOnda.setString("Onda: " + std::to_string(ondaAtual));

            janela.draw(textoOnda);

            if (aguardandoProximaOnda)
            {
                janela.draw(textoTransicao);
            }

            // Atualizar indicador do tiro duplo
            if (tiroDuploAtivo)
            {
                float tempoRestante = 8.f - tempoTiroDuplo;

                if (tempoRestante < 0.f)
                {
                    tempoRestante = 0.f;
                }

                textoTiroDuplo.setString(
                    "TIRO DUPLO: " +
                    std::to_string(static_cast<int>(std::ceil(tempoRestante))) +
                    "s"
                    );
            }
            else
            {
                textoTiroDuplo.setString("");
            }

            janela.draw(textoTiroDuplo);

            if (tiroRapidoAtivo)
            {
                float tempoRestante = 8.f - tempoTiroRapido;

                if (tempoRestante < 0.f)
                    tempoRestante = 0.f;

                textoTiroRapido.setString(
                    "TIRO RAPIDO: " +
                    std::to_string(
                        static_cast<int>(std::ceil(tempoRestante))
                        ) +
                    "s"
                    );
            }
            else
            {
                textoTiroRapido.setString("");
            }

            janela.draw(textoTiroRapido);

            // =========================================
            // DESENHAR MENSAGEM DE PAUSA
            // =========================================
            if (jogoPausado)
            {
                // Fundo semitransparente sobre a partida
                sf::RectangleShape fundoPausa(
                    sf::Vector2f(800.f, 600.f)
                    );

                fundoPausa.setFillColor(
                    sf::Color(0, 0, 0, 150)
                    );

                janela.draw(fundoPausa);

                // Texto da pausa
                sf::Text textoPausado;
                textoPausado.setFont(fonte);
                textoPausado.setString("PAUSADO");
                textoPausado.setCharacterSize(48);
                textoPausado.setFillColor(sf::Color::White);

                sf::FloatRect limitesTexto =
                    textoPausado.getLocalBounds();

                textoPausado.setOrigin(
                    limitesTexto.left + limitesTexto.width / 2.f,
                    limitesTexto.top + limitesTexto.height / 2.f
                    );

                textoPausado.setPosition(400.f, 300.f);

                janela.draw(textoPausado);
            }
        }
        else
        {
            // Game Over e instrução para reiniciar
            // Mostrar GAME OVER

            janela.draw(textoGameOver);
            janela.draw(textoReiniciar);

        }

        janela.display();
    }
}