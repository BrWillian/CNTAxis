# CNTAxis

Biblioteca C++ para detecção de veículos e contagem/associação de eixos usando ONNX Runtime e OpenCV.

## O que resolve
- **Contagem de eixos por veículo** em imagens, associando eixos corretamente ao veículo correspondente.
- Útil para **pedágios**, **balanças rodoviárias**, **fiscalização** e **analytics** de tráfego.

## Como usar (rápido)
1. **Modelo**: coloque seu modelo ONNX em `models/model.onnx` ou ajuste o caminho no código.
2. **Compile**:
   ```bash
   cmake -S . -B build
   cmake --build build -j
   ```
3. **Execute a demo**:
   ```bash
   ./bin/debug sua_imagem.jpg
   ```
4. **Resultado**: uma janela é aberta mostrando os veículos com caixas e o **número de eixos** anotado.

## Entrada e saída
- **Entrada**: imagem comum (JPG/PNG) da via/câmera.
- **Saída**: lista de veículos detectados com o número de eixos por veículo; a demo exibe a imagem anotada.

## Limitações
- A precisão depende do **modelo ONNX** utilizado e da **qualidade das imagens**.
- Iluminação, chuva, ângulo de câmera e oclusões podem afetar resultados.
- Imagens muito grandes podem reduzir a velocidade; redimensionar ajuda.

## Integração no seu sistema
- Use a biblioteca gerada `bin/libCNTAxis.so` e a classe `CNTCore` para obter os eixos: `getAxis(frame)`.
- Há um exemplo mínimo em `tests/debug.cpp` que você pode adaptar.

## Visão geral
- **Biblioteca**: `CNTAxis` (compartilhada) exposta via C++.
- **Detecção**: modelo ONNX (YOLO-like) com duas classes: `vehicle` e `axis`.
- **Pós-processamento**: NMS, remapeamento de coordenadas e associação de eixos a cada veículo.
- **Executável de depuração**: `bin/debug` para testar em uma imagem.

## Estrutura do projeto
- **`include/`**: headers públicos
  - `ONNXModel.h`: wrapper genérico do ONNX Runtime.
  - `CNTAxis.h`: especialização que define classes [`vehicle`, `axis`].
  - `CNTCore.h`: orquestra pré-processamento, inferência e associação de eixos.
  - `ImageOp.h`: utilitários de imagem (pré-processamento e blob).
- **`src/`**: implementações.
- **`meta/`**: tipos (`Types.h`) e rascunho de wrapper C (`C_Wrapper.h`).
- **`tests/`**: `debug.cpp` (exemplo de uso CLI).
- **`models/`**: modelo ONNX de exemplo (ex.: `model.onnx`).
- **`CMakeLists.txt`**: build da lib e do executável.
- **`Dockerfile`**: ambiente reprodutível (OpenCV estático + ONNX Runtime).

## Dependências
- C++20
- OpenCV (core, imgproc, dnn, etc.)
- ONNX Runtime
- Threads (padrão do sistema)

Observações de linkedição/paths:
- O `CMakeLists.txt` espera `ONNX Runtime` estático em: `/usr/local/lib/libonnxruntime.a`.
- O output (binários e libs) é gerado em `bin/`.

## Build local (CMake)
```bash
# Na raiz do projeto
cmake -S . -B build
cmake --build build -j
```
Artefatos gerados em `bin/`:
- `bin/libCNTAxis.so` (biblioteca)
- `bin/debug` (executável de teste)

Caso falte o ONNX Runtime em `/usr/local/lib/libonnxruntime.a`, use o Docker abaixo ou instale manualmente.

## Usando Docker (ambiente de build)
O `Dockerfile` monta OpenCV (estático) e ONNX Runtime e consolida em libs estáticas.

```bash
# Construir a imagem
docker build -t cntaxis:dev .

# Rodar o container com o código montado
docker run --rm -it -v "$PWD":/workspace -w /workspace cntaxis:dev /bin/bash

# Dentro do container
cmake -S . -B build
cmake --build build -j
```

## Modelo ONNX
- Exemplo de localização do modelo: `models/model.onnx`.
- Inicialização do `CNTCore`:
  - Construtor padrão usa `./models/axis.onnx` (veja `CNTCore.cpp`).
  - O teste `debug.cpp` usa `./model.onnx` passado no construtor. Você pode:
    - Copiar/renomear seu modelo para `./models/axis.onnx`; ou
    - Instanciar `CNTCore` com o caminho explícito do seu modelo.

## Exemplo de uso (CLI)
Após compilar, rode o executável de depuração com uma imagem:
```bash
./bin/debug caminho/para/imagem.jpg
```
Ele irá:
- Carregar o modelo informado no código de teste (`tests/debug.cpp`).
- Detectar veículos e eixos.
- Desenhar caixas e rótulos na imagem e mostrar uma janela com o resultado.

## API principal
- `ONNXModel` (`include/ONNXModel.h`)
  - `ONNXModel(std::string model_path)` e `ONNXModel(const unsigned char*, unsigned int)`
  - `float* infer(const std::vector<float>&)`
  - `std::vector<Detection> postProcess(float*)` (aplica NMS e gera `Detection`)
- `CNTAxis` (`include/CNTAxis.h`)
  - Especializa `ONNXModel` e define classes `{"vehicle", "axis"}`
- `CNTCore` (`include/CNTCore.h`)
  - `std::vector<Vehicle> getAxis(const cv::Mat& frame) const`
    - Faz pré-processamento via `ImageOperator`, inferência com `CNTAxis` e associação de eixos.
  - `static void associateAxisToVehicle(std::vector<Vehicle>&, const std::vector<Axis>&)`
  - `static float calculateIou(const cv::Rect&, const cv::Rect&)`
- Tipos (`meta/Types.h`)
  - `Detection { int classId; float confidence; cv::Rect bbox; }`
  - `Axis { cv::Rect bbox; float confidence; }`
  - `Vehicle { cv::Rect bbox; float confidence; int numAxis; std::vector<Axis> axis; }`

## Observações sobre o wrapper C
- `meta/C_Wrapper.h` declara uma API C (`C_CNTAXISCREATE`, `C_CNTAXISINFERENCE`, etc.).
- As implementações não estão presentes no repositório no momento. Use a API C++ diretamente até que as definições sejam adicionadas.

## Notas de desempenho
- A sessão do ONNX Runtime faz warm-up na inicialização (`ONNXModel::WarmUpSession`).
- Pré-processamento mantém aspect ratio com padding e converte BGR→RGB.

## Licença
Defina a licença do projeto conforme necessário (por exemplo, MIT/Apache-2.0). Adicione um arquivo `LICENSE` se aplicável.
