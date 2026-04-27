import axios from 'axios';

const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:5000';

export async function runCode({ code, language }) {
  const response = await axios.post(
    `${API_URL}/run`,
    { code, language },
    {
      headers: { 'Content-Type': 'application/json' },
      timeout: 30000, // 30s timeout
    }
  );
  return response.data;
}
