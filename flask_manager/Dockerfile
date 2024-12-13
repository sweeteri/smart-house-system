# Используем официальный Python-образ
FROM python:3.9-slim

# Устанавливаем рабочую директорию
WORKDIR /app

# Копируем файл зависимостей и устанавливаем их
COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

# Копируем все файлы проекта в контейнер
COPY . .

# Открываем порты для Flask
EXPOSE 5000

# Запускаем сервер Flask
CMD ["python", "app.py"]
