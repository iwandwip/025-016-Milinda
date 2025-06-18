import RegisterForm from "@/components/auth/register-form";
import Link from "next/link";

export default function RegisterPage() {
  return (
    <div className="min-h-screen flex items-center justify-center bg-gradient-to-br from-blue-50 to-indigo-100 dark:from-gray-900 dark:to-gray-800">
      <div className="w-full max-w-md px-4">
        <RegisterForm />
        <div className="text-center mt-4">
          <p className="text-sm text-gray-600 dark:text-gray-400">
            Sudah punya akun?{" "}
            <Link href="/login" className="text-blue-600 hover:text-blue-500">
              Login di sini
            </Link>
          </p>
        </div>
      </div>
    </div>
  );
}