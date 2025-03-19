from flask import Flask, request, jsonify
import google.generativeai as genai
import os
import json

app = Flask(__name__)

# Load your API key from an environment variable (recommended)
GOOGLE_API_KEY = "AIzaSyAQNTmaW31-Mw8BgQTbklvmv3bRk0kSCTo"
if not GOOGLE_API_KEY:
    raise ValueError("No GOOGLE_API_KEY environment variable set.")

genai.configure(api_key=GOOGLE_API_KEY)

# Select the model
model = genai.GenerativeModel('gemini-1.5-flash') # or 'gemini-pro-vision' if you're using images

@app.route('/generate', methods=['POST'])
def generate_text():
    print("Received a request to /generate")  # Add this line
    data = request.get_json()
    prompt = data.get('prompt')  # Get the prompt from the request body

    if not prompt:
        return jsonify({"error": "Prompt is required"}), 400

    try:
        response = model.generate_content(prompt)
        print(f"Raw response from Gemini API: {response.text}")  # Print the raw text
        print("Gemini API call successful") # Add this line
        json_string = response.text
        cleaned_json_data = json_string.replace("```json", "").replace("```", "").replace("\n", "")
        json_data = json.loads(cleaned_json_data)
        #return jsonify({"response": response.text})  # Send back the generated text
        return jsonify(json_data)
        #return response.text
    except Exception as e:
        print(f"Error generating text: {e}")
        return jsonify({"error": str(e)}), 500  # Handle errors gracefully

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')  # Only use debug=True during development!